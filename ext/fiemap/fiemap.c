#include <ruby.h>

VALUE cFIEMAP;

static VALUE
dummy(void) {
    rb_p(Qtrue);
    return Qnil;
}

void Init_fiemap(void) {
   cFIEMAP = rb_define_class("FIEMAP", rb_cObject); 
   rb_define_method(cFIEMAP, "dummy", dummy, 0);
}
