decribe File::Extent do
  before do
    @t = Tempfile.new("fiemap", "w")    
    @t.write("a" * 4096)
  end

  it do
    p @t
  end

  after do
    @t.close unless t.closed!
  end
end
