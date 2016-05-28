# == kernel_spec.rb
#
# Basic tests for KernelHandler class These should load first, as they're
# essential to performing any meaningful task with the SPICE Toolkit
#
# TODO : Add tests for the direct SpiceRub::furnsh(),ktotal(), functions.
#          These functions won't be directly exposed as of now.

require "spec_helper"

describe SpiceRub::KernelHandler do
  before :all do
    @kernel_pool = SpiceRub::KernelHandler.new
  end

  it "can only be instantiated once" do
    expect { second_instance = SpiceRub::KernelHandler.new }.to raise_exception
  end
  
  it "raises an exception when loading an invalid kernel file" do
    expect { @kernel_pool.load("data/kernels/invalid_kernel.txt")}.to raise_exception
  end

  context "#load" do
    it "successfully loads a text leap second kernel file (.tls)" do
      kernel1 = @kernel_pool.load("data/kernels/naif0011.tls")
      expect(@kernel_pool.filename(kernel1)).to eq("naif0011.tls")
    end

    it "successfully loads a binary SPK kernel file (.bsp)" do
      kernel2 = @kernel_pool.load("data/kernels/de405_1960_2020.bsp")
      expect(@kernel_pool.filename(kernel2)).to eq("de405_1960_2020.bsp")
    end   	

    it "successfully loads a binary PCK kernel file (.bpc)" do
      kernel3 = @kernel_pool.load("data/kernels/moon_pa_de421_1900-2050.bpc")
      expect(@kernel_pool.filename(kernel3).to eq("moon_pa_de421_1900-2050.bpc")  
    end

    #TODO
    # Add more tests for different kernel files
  end
  
  context "#total" do
    it "returns the total number of kernel files loaded into the kernel pool" do
      total = @kernel_pool.total
      
      expect(total).to eq(3)
    end

    it "returns the number of kernel files loaded belonging to the category specified" do
      spk_total = @kernel_pool.total(:SPK)
      pck_total = @kernel_pool.total(:PCK)
      
      expect(spk_total).to eq(1)
      expect(pck_total).to eq(1)
    end

    it "returns the number of text kernel files loaded" do
      text_total = @kernel_pool.total(:TEXT)
      expect(text_total).to eq(1)
    end    
  end

  context "unloading kernel files" do
    it "successfully unloads a kernel file from the kernel pool specified by its index in the pool" do
      status = @kernel_pool.unload(3)
      
      expect(status).to be_true
      expect(@kernel_pool.filename(3)).to be_nil
      expect(@kernel_pool.total).to eq(2)
    end

    it "clears the entire kernel pool" do
      @kernel_pool.clear
      expect(@kernel_pool.list).to be_nil
    end
  end
end
