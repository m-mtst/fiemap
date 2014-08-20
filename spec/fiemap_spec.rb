require_relative "spec_helper"

describe "File#extents" do
  context "one extent" do
    let(:tempfile) { Tempfile.new("fiemap_spec") }

    before { tempfile.write("a" * 4096) }

    subject { tempfile.extents }

    it { is_expected.to be_a Array }
    it { expect(subject.size).to eq(1) }

    after do
      tempfile.close unless tempfile.closed?
    end
  end

  context "two extent" do
    let(:tempfile) { Tempfile.new("fiemap_spec") }

    before do
      tempfile.write("a" * 4096)
      tempfile.seek(4096, :CUR)
      tempfile.write("a" * 4096)
    end

    subject { tempfile.extents }

    it { p subject }
    it { is_expected.to be_a Array }
    it { expect(subject.size).to eq(2) }

    after do
      tempfile.close unless tempfile.closed?
    end
  end
end

describe File::Extent do
  let(:tempfile) { Tempfile.new("fiemap_spec") }

  before { tempfile.write("a" * 4096) }

  subject { tempfile.extents }

  it { is_expected.to be_a Array }
  it { expect(subject.size).to eq(1) }

  after do
    tempfile.close unless tempfile.closed?
  end
end
