#include <ruby.h>
#include <ruby/io.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/fiemap.h>

VALUE cExtent;

enum {
    MAX_EXTENT = 64
};

static size_t
extent_memsize(const void *p)
{
    return p ? sizeof(struct fiemap_extent) : 0;
}

static const rb_data_type_t extent_data_type = {
    "stat",
    {NULL, RUBY_TYPED_DEFAULT_FREE, extent_memsize,},
    NULL, NULL, RUBY_TYPED_FREE_IMMEDIATELY
};

static VALUE
extent_new(VALUE klass, const struct fiemap_extent *extent)
{
    struct fiemap_extent *new_extent = NULL;

    if (extent) {
	new_extent = ALLOC(struct fiemap_extent);
	*new_extent = *extent;
    }
    return TypedData_Wrap_Struct(klass, &extent_data_type, new_extent);
}

static VALUE
extent_alloc(VALUE klass)
{
    return extent_new(klass, NULL);
}

static const struct fiemap_extent *
get_extent(VALUE self)
{
    const struct fiemap_extent *extent;
    TypedData_Get_Struct(self, struct fiemap_extent, &extent_data_type, extent);
    return extent;
}

static VALUE
extent_logical_offset(VALUE self)
{
    return LONG2NUM(get_extent(self)->fe_logical);
}

static VALUE
extent_physical_offset(VALUE self)
{
    return LONG2NUM(get_extent(self)->fe_physical);
}

static VALUE
extent_length(VALUE self)
{
    return LONG2NUM(get_extent(self)->fe_length);
}

static VALUE
extent_flags(VALUE self)
{
    return INT2FIX(get_extent(self)->fe_flags);
}

static VALUE
extent_test_flag(VALUE self, int flag)
{
    if (get_extent(self)->fe_flags & flag) {
	return Qtrue;
    }
    return Qfalse;
}

static VALUE
extent_last_p(VALUE self)
{
    return extent_test_flag(self, FIEMAP_EXTENT_LAST);
}

static VALUE
extent_unknown_p(VALUE self)
{
    return extent_test_flag(self, FIEMAP_EXTENT_UNKNOWN);
}

static VALUE
extent_delalloc_p(VALUE self)
{
    return extent_test_flag(self, FIEMAP_EXTENT_DELALLOC);
}

static VALUE
extent_encoded_p(VALUE self)
{
    return extent_test_flag(self, FIEMAP_EXTENT_ENCODED);
}

static VALUE
extent_encrypted_p(VALUE self)
{
    return extent_test_flag(self, FIEMAP_EXTENT_DATA_ENCRYPTED);
}

static VALUE
extent_not_aligned_p(VALUE self)
{
    return extent_test_flag(self, FIEMAP_EXTENT_NOT_ALIGNED);
}

static VALUE
extent_inline_p(VALUE self)
{
    return extent_test_flag(self, FIEMAP_EXTENT_DATA_INLINE);
}

static VALUE
extent_tail_p(VALUE self)
{
    return extent_test_flag(self, FIEMAP_EXTENT_DATA_TAIL);
}

static VALUE
extent_unwritten_p(VALUE self)
{
    return extent_test_flag(self, FIEMAP_EXTENT_UNWRITTEN);
}

static VALUE
extent_merged_p(VALUE self)
{
    return extent_test_flag(self, FIEMAP_EXTENT_MERGED);
}

#define rb_sys_fail_path(path) rb_sys_fail(NIL_P(path) ? 0 : RSTRING_PTR(path))

static VALUE
rb_file_extents(VALUE io)
{
    unsigned int i;
    VALUE temp;
    rb_io_t *fptr;
    __u64 end, start = 0;
    struct stat statinfo;
    VALUE extents = Qnil;
    struct fiemap *p_filemap = (struct fiemap *)xmalloc(offsetof(struct fiemap, fm_extents[0]) + sizeof(struct fiemap_extent) * MAX_EXTENT);

    temp = rb_io_check_io(io);
    GetOpenFile(temp, fptr);

    if (fstat(fptr->fd, &statinfo) < 0) {
	rb_sys_fail_path(fptr->pathv);
    }

    end = statinfo.st_size;
    
    while (start < end) {
        p_filemap->fm_start  = start;
        p_filemap->fm_length = end;
        p_filemap->fm_flags = 0;
        p_filemap->fm_extent_count = MAX_EXTENT;

	if (ioctl(fptr->fd, FS_IOC_FIEMAP, p_filemap) == -1) {
	    rb_sys_fail_path(fptr->pathv);
	}

	if (NIL_P(extents)) {
	    extents = rb_ary_new_capa(p_filemap->fm_mapped_extents);
	}
	else {
	    rb_ary_resize(extents, RARRAY_LEN(extents) + p_filemap->fm_mapped_extents);
	}

        for (i=0; i < p_filemap->fm_mapped_extents; i++) {
            struct fiemap_extent extent;

            extent = p_filemap->fm_extents[i];
	    rb_ary_push(extents, extent_new(cExtent, &extent));
            start = extent.fe_logical + extent.fe_length;
        }
    }

    return extents;
}

void Init_fiemap(void) {
    cExtent = rb_define_class_under(rb_cFile, "Extent", rb_cObject);
    rb_define_alloc_func(cExtent, extent_alloc);

    rb_define_method(cExtent, "offset", extent_logical_offset, 0);
    rb_define_method(cExtent, "physical_offset", extent_physical_offset, 0);
    rb_define_method(cExtent, "length", extent_length, 0);
    rb_define_method(cExtent, "flags", extent_flags, 0);
    rb_define_alias(cExtent, "size", "length");

    rb_define_const(cExtent, "FIEMAP_EXTENT_LAST", INT2FIX(FIEMAP_EXTENT_LAST));
    rb_define_const(cExtent, "FIEMAP_EXTENT_UNKNOWN", INT2FIX(FIEMAP_EXTENT_UNKNOWN));
    rb_define_const(cExtent, "FIEMAP_EXTENT_DELALLOC", INT2FIX(FIEMAP_EXTENT_DELALLOC));
    rb_define_const(cExtent, "FIEMAP_EXTENT_ENCODED", INT2FIX(FIEMAP_EXTENT_ENCODED));
    rb_define_const(cExtent, "FIEMAP_EXTENT_DATA_ENCRYPTED", INT2FIX(FIEMAP_EXTENT_DATA_ENCRYPTED));
    rb_define_const(cExtent, "FIEMAP_EXTENT_NOT_ALIGNED", INT2FIX(FIEMAP_EXTENT_NOT_ALIGNED));
    rb_define_const(cExtent, "FIEMAP_EXTENT_DATA_INLINE", INT2FIX(FIEMAP_EXTENT_DATA_INLINE));
    rb_define_const(cExtent, "FIEMAP_EXTENT_DATA_TAIL", INT2FIX(FIEMAP_EXTENT_DATA_TAIL));
    rb_define_const(cExtent, "FIEMAP_EXTENT_UNWRITTEN", INT2FIX(FIEMAP_EXTENT_UNWRITTEN));
    rb_define_const(cExtent, "FIEMAP_EXTENT_MERGED", INT2FIX(FIEMAP_EXTENT_MERGED));

    rb_define_method(cExtent, "last?", extent_last_p, 0);
    rb_define_method(cExtent, "delalloc?", extent_delalloc_p, 0);
    rb_define_method(cExtent, "encoded?", extent_encoded_p, 0);
    rb_define_method(cExtent, "encrypted?", extent_encrypted_p, 0);
    rb_define_method(cExtent, "not_aligned?", extent_not_aligned_p, 0);
    rb_define_method(cExtent, "inline?", extent_inline_p, 0);
    rb_define_method(cExtent, "tail?", extent_tail_p, 0);
    rb_define_method(cExtent, "unwritten?", extent_unwritten_p, 0);
    rb_define_method(cExtent, "merged?", extent_merged_p, 0);

    rb_define_method(rb_cFile, "extents", rb_file_extents, 0);
}
