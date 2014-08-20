require "fiemap/version"
require "fiemap/fiemap"

class File
  class Extent
    private_class_method :new

    def inspect
      "#<#{self.class} logical:#{self.offset} physical:#{self.physical_offset} flags:#{flags}>"
    end

    private

    def flags
      flags = []
      flags << "last" if last?
      flags << "delalloc" if delalloc?
      flags << "encoded" if encoded?
      flags << "encrypted" if encrypted?
      flags << "not_aligned" if not_aligned?
      flags << "inline?" if inline?
      flags << "tail" if tail?
      flags << "unwritten" if unwritten?
      flags << "merged" if merged?
      flags.join(",")
    end
  end
end
