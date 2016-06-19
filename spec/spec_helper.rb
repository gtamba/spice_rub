#TODO : Overrides its to accept function parameters
require 'nmatrix'
require './lib/spice_rub'

#Include can be moved to separate specs if not used frequently eventually
include Math

TEST_SPK_KERNEL = "de405_1960_2020.bsp"
TEST_TLS_KERNEL  = "naif0011.tls"
TEST_PCK_KERNEL = ["moon_pa_de421_1900-2050.bpc", "pck00010.tpc"]
TEST_INVALID_KERNEL = "invalid_kernel.txt"
TEST_IK_KERNEL = "instrument.ti"

TEST_KERNELS = { tls: TEST_TLS_KERNEL, bsp: TEST_SPK_KERNEL, pck: TEST_PCK_KERNEL[0]}
TEST_POOL = [TEST_INVALID_KERNEL, TEST_TLS_KERNEL, TEST_SPK_KERNEL, TEST_PCK_KERNEL[0] ]


TEST_INSTRUMENTS = [-999001, -999002]
#TODO : Overrides its to accept function parameters
module OverrideSPec
  def its(*args)
    
  end

  RSpec.configure { |c| c.extend self }
end

## Custom RSpec matcher ary_be_within() contributed by Victor Shepelev, @zverok
## 
##   Matches two arrays within a roundoff error between their floating point values   
##
RSpec::Matchers.define :ary_be_within do |delta|
  match do |actual|
    unless @expected
      raise ArgumentError, "This matcher should be used as ary_be_withing(#{delta}).of(expected_value)"
    end

    if actual.size != @expected.size
      raise ArgumentError, "Actual and expected array sizes differ: #{actual.size} vs #{@expected.size}"
    end

    actual.zip(@expected).all? { |a, e| (a - e).abs < delta }
  end

  def of(expected)
    @expected = expected

    self
  end

  failure_message do |actual|
    "expected #{actual} to be withing #{delta} of #{@expected}"
  end
end