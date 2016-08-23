# == kernel_spec.rb
#
# Basic tests for KernelPool class These should load first, as they're
# essential to performing any meaningful task with the SPICE Toolkit
#
# TODO : Add tests for the direct SpiceRub::furnsh(),ktotal(), functions.
#          These functions won't be directly exposed as of now.

require "spec_helper"

describe SpiceRub::KernelPool do
 
  let(:kernel_pool) { SpiceRub::KernelPool.instance }
  before { kernel_pool.path = 'spec/data/kernels' }
  before(:each) { kernel_pool.clear! unless kernel_pool.empty? }
  
  context "When instantiating with a constructor" do
    
    def new_instance
      SpiceRub::KernelPool.new
    end
    
    it "raises a NoMethodError error" do
      expect { new_instance }.to raise_error(NoMethodError)  
    end
  end
  
  TEST_KERNELS.each do |type, file|
    context "Loading #{type} kernel" do
      before { kernel_pool.load(file) }
      subject { kernel_pool[0] }
      
      #be_a_instance_of fails for some reason
      it { is_expected.to be_a (SpiceRub::SpiceKernel) }
      it { is_expected.to be_loaded }
      
      #its is deprecated 
      its(:path_to) {is_expected.to eq File.join(kernel_pool.path, file)}
    end
  end
  
  describe "#count" do
    before { TEST_KERNELS.each {|type,file| kernel_pool.load(file)} }
    
    context "When no category is specified" do
      subject { kernel_pool.count }

      it { is_expected.to eq 3 }
    end
    
    context "When category is :spk"
      subject { kernel_pool.count(:spk) }

      it { is_expected.to eq 1}

    context "When category is :pck" do
      subject { kernel_pool.count(:pck) }

      it { is_expected.to eq 1}
    end

    context "When category is :text" do
      subject { kernel_pool.count(:text) }

      it { is_expected. to eq 1}
    end
  end
  
  context "When unloading all kernels at once" do
    subject { kernel_pool }
    before do 
      TEST_KERNELS.each {|type,file| kernel_pool.load(file)} 
      subject.clear!
    end

    its(:count) { is_expected.to eq 0 }
    it { is_expected.to be_empty }
  end  
  
  context "When unloading the only kernel" do
    subject { kernel_pool }
    before  do 
      kernel = subject.load(TEST_KERNELS.first[1])
      subject[kernel].unload!
    end

    its(:count) {is_expected.to eq 0}
    it { is_expected.to be_empty }
  end
  
  context "When loading all the kernels from @path without specifying folder" do
    let(:expected) { Dir[File.join(kernel_pool.path, "*.*")].size }
    subject { kernel_pool.load_folder }
    
    #This test output will change as more files are added to spec/data/kernels  
    it { is_expected.to eq expected }
  end
  
  context "When loading all the kernels from a specified folder" do
    let(:expected) { Dir[File.join(kernel_pool.path, "*.*")].size }
    subject { kernel_pool.load_folder("spec/data/kernels") }
    
    #This test output will change as more files are added to spec/data/kernels  
    it { is_expected.to eq expected }
  end

  # Failing test
  context "When a SpiceKernel gets unloaded" do

  end

end

