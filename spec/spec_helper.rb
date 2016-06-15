#TODO : Overrides its to accept function parameters
require 'nmatrix'
require './lib/spice_rub'

TEST_SPK_KERNEL = "de405_1960_2020.bsp"
TEST_TLS_KERNEL  = "naif0011.tls"
TEST_PCK_KERNEL = ["moon_pa_de421_1900-2050.bpc", "pck00010.tpc"]
TEST_INVALID_KERNEL = "invalid_kernel.txt"
TEST_IK_KERNEL = "instrument.ti"

TEST_KERNELS = { tls: "naif0011.tls", bsp: "de405_1960_2020.bsp", pck: "moon_pa_de421_1900-2050.bpc"}
TEST_POOL = [TEST_INVALID_KERNEL, TEST_TLS_KERNEL, TEST_SPK_KERNEL, TEST_PCK_KERNEL]


TEST_INSTRUMENTS = [-999001, -999002]
#TODO : Overrides its to accept function parameters
module OverrideSPec
  def its(*args)
    
  end

  RSpec.configure { |c| c.extend self }
end

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