# Fiemap

FIEMAP ioctl wrapper for Ruby.
It adds File#extents and File::Extent class.

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'fiemap'
```

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install fiemap

## Usage

```ruby
require "fiemap"

f = File.open("something")
extents = f.extents #=> [#<File::Extent logical:0 physical:0 flags:delalloc>, #<File::Extent logical:8192 physical:0 flags:last,delalloc>]

extent = extents.last
extent.last? #=> true
extent.delalloc? #=> true
extent.inline? #=> false

extent.flags & File::Extent::FIEMAP_EXTENT_LAST #=> 1
```

## File::Extent

### Instance Methods

* last?
* delalloc?
* encoded?
* encrypted?
* not_aligned?
* inline?
* tail?
* unwritten?
* merged?

### Constants

* FIEMAP_EXTENT_LAST
* FIEMAP_EXTENT_UNKNOWN
* FIEMAP_EXTENT_DELALLOC
* FIEMAP_EXTENT_ENCODED
* FIEMAP_EXTENT_DATA_ENCRYPTED
* FIEMAP_EXTENT_NOT_ALIGNED
* FIEMAP_EXTENT_DATA_INLINE
* FIEMAP_EXTENT_DATA_TAIL
* FIEMAP_EXTENT_UNWRITTEN
* FIEMAP_EXTENT_MERGED

## Contributing

1. Fork it ( https://github.com/[my-github-username]/fiemap/fork )
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request
