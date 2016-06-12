require 'nmatrix'
require './lib/spice_rub'

TEST_SPK_KERNEL = "data/kernels/de405_1960_2020.bsp"
TEST_TLS_KERNEL  = "data/kernels/naif0011.tls"
TEST_PCK_KERNEL = "data/kernels/moon_pa_de421_1900-2050.bpc"
TEST_INVALID_KERNEL = "data/kernels/invalid_kernel.txt"

TEST_KERNELS = {:TLS => "naif0011.tls", :BSP => "de405_1960_2020.bsp", :PCK => "moon_pa_de421_1900-2050.bpc"}
TEST_POOL = [TEST_INVALID_KERNEL, TEST_TLS_KERNEL, TEST_SPK_KERNEL, TEST_PCK_KERNEL]

#TODO : Overrides its to accept function parameters
module OverrideSPec
  def its(*args)
    
  end

  RSpec.configure { |c| c.extend self }
end