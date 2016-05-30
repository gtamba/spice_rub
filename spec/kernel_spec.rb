# == kernel_spec.rb
#
# Basic tests for KernelHandler class These should load first, as they're
# essential to performing any meaningful task with the SPICE Toolkit
#
# TODO : Add tests for the direct SpiceRub::furnsh(),ktotal(), functions.
#          These functions won't be directly exposed as of now.

require "spec_helper"

describe SpiceRub::KernelPool do
  
  context "when trying to instantiate with constructor"
    
    def second_instance
      SpiceRub::KernelPool.new
    end
    
    expect { second_instance }.to raise_error(NoMethodError)  
    #TODO : Add article to readme about KernelPool being Singleton
  end

  def clear_state(test_object)
    test_object.clear! unless test_object.empty?
  end

  describe "#load" do
    let (:kernel_pool) {SpiceRub::KernelPool.instance}
    
    context "when input is an invalid kernel" do
      subject { kernel_pool }
      before  { clear_state(kernel_pool)}
      
      it "raises an ArgumentError exception" do
        expect { subject.load(TEST_INVALID_KERNEL)}.to raise_exception(ArgumentError)
      end
    end

    context "when input is a TLS Kernel" do
      subject { kernel_pool }
      before  { clear_state(kernel_pool)
                kernel_pool.load(TEST_TLS_KERNEL) }

      its(:[], 0) { is_expected.to eq test_pool[:TLS] }
    end

    context "when input is a PCK Kernel" do
      subject { kernel_pool }
      before  { clear_state(kernel_pool)
                kernel_pool.load(TEST_PCK_KERNEL) }

      its(:[], 0) { is_expected.to eq test_pool[:PCK] }
    end

    context "when input is a SPK Kernel" do
      subject { kernel_pool }
      before  { clear_state(kernel_pool)
                kernel_pool.load(TEST_SPK_KERNEL) }

      its(:[], 0) { is_expected.to eq test_pool[:SPK] } 
    end
  end
  
  describe "#total" do
    let (:kernel_pool) {SpiceRub::KernelPool.instance}
    before { 0.upto(2) do |kernel| { kernel_pool.load(test_kernels[kernel])}}

    context "when category is :ALL or unspecificed" do
      subject { kernel_pool }
      
      its(:total) { is_expected.to eq 3}
    end

    context "when category is :SPK" do
      subject { kernel_pool }

      its(:total, :SPK) { is_expected.to eq 1}
    end

    context "when category is :TLS" do
      subject { kernel_pool }

      its(:total, :TLS) { is_expected.to eq 1}
    end

    context "when category is :PCK" do
      subject { kernel_pool }

      its(:total, :SPK) { is_expected.to eq 1}
    end
  end

  describe "#unload!" do
    context "when unloading a kernel file" do
      let(:kernel_pool) {SpiceRub::KernelPool.instance}
      before { clear_state(kernel_pool) 
               kernel_pool.load(TEST_TLS_KERNEL) }

      its(:unload!) {is_expected.to respond_with 0}                         
      its(:total) {is_expected.to eq 0}
    end
  end
    
  describe "#clear!" do
    context "when unloading a kernel file" do
      let(:kernel_pool) {SpiceRub::KernelPool.instance}
      before { clear_state(kernel_pool) 
               0.upto(2) do |kernel| { kernel_pool.load(test_kernels[kernel])}}

      its(:clear!) {is_expected.to respond_with 0}                         
      its(:total) {is_expected.to eq 0}
      its(:empty?){is_expected.to respond_with true}
    end
  end
end
