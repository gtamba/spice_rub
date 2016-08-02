# == body_spec.rb
#
# Basic tests for Body class , main class for ephemeris related tasks
# 
#

require "spec_helper"

describe SpiceRub::Body do

  let(:kernel_pool) { SpiceRub::KernelPool.instance }
  let(:test_body)   { SpiceRub::Body.new(:earth) }

  before { kernel_pool.path = 'spec/data/kernels' }
  before(:each) { kernel_pool.clear! unless kernel_pool.empty? }

  describe ".new" do
    
    context "When instantiating with body integer code" do
      subject { SpiceRub::Body.new(399) }

      it { is_expected.to be_instance_of(SpiceRub::Body) }
      its(:code) { is_expected.to be eq 399 }
      its(:name) { is_expected.to be eq :earth }
    end

    context "When instantiating with body symbol name" do
      subject { SpiceRub::Body.new(:moon) }

      it { is_expected.to be_instance_of(SpiceRub::Body) }
      its(:code) { is_expected.to be eq 301 }
      its(:name) { is_expected.to be eq :moon }

    end

    context "When instantiating with body symbol name and frame of reference" do
      subject { SpiceRub::Body.new(:earth, frame: :ECLIPJ2000) }

      it { is_expected.to be_instance_of(SpiceRub::Body) }
      its(:code) { is_expected.to be eq 399 }
      its(:name) { is_expected.to be eq :earth }
    end
  end   
  
  describe "#name" do
    subject { test_body.name }

    it { is_expected.to be eq :earth}
  end

  describe "#code" do
    subject { test_body.code }

    it { is_expected.to be eq 399 }
  end

  describe "#type" do
    subject { test_body.type }

    it { is_expected.to eq :planet }
  end
  
  # Body#position_at( time , observer = :SUN , frame = :J2000, aberration_correction = none, with_light_distanct: nil)
  #
  describe "#position_at" do
    
    context "When all parameters are specified" do
      let(:expected) {  NMatrix.new([3,1], [-97579460.22494915, 
                                            -111325884.19041583, 
                                            -53609500.96053864]) 
                     }
      
      subject { Body.new(:moon).position_at("2006 JAN 31 01:00", observer: :MARS, frame: :J2000, aberration_correction: :NONE) }
      
      it { is_expected.to be_within(0.00000001).of expected }
    end

    context "When computing position of a body at an epoch" do
      let(:expected) { [
                        NMatrix.new([3,1] , [
                                            -26468760.60299247,
                                            132758515.52645776,
                                             57556706.30607039
                                            ] ) ,
                        490.6703256499084
                       ]
                     }
      
      context "when epoch is a time string" do
        subject { test_body.position_at("2002", with_light_time: true) }
      
        it { is_expected.to ary_be_within(0.00001).of expected }
      end  
      
      context "when epoch is seconds past J2000 Epoch" do
        subject { test_body.position_at(63115264.183926724, with_light_time: true) }
      
        it { is_expected.to ary_be_within(0.00001).of expected }
      end    
    end

    context "When computing position_at of a body at multiple epochs in input array" do
      let(:expected) { }

      subject { test_body.position_at([63115264.183926724, "2003", "2004", "2005"]) }

      it { is_expected.to be_within(0.00001).of expected } 
    end
  end

  describe "#state_at" do

    context "When all parameters are specified" do
      let(:expected) {  NMatrix.new([6,1], [-97579460.22494915, 
                                            -111325884.19041583, 
                                            -53609500.96053864,
                                             191941265.18476546, 
                                                              0, 
                                                              0
                                           ] ) 
                     }
      
      subject { Body.new(:moon).state_at("2006 JAN 31 01:00", observer: :MARS, frame: :J2000, aberration_correction: :NONE) }
      
      it { is_expected.to be_within(0.00000001).of expected }
    end

    context "When computing state of a body at an epoch" do
      let(:expected) { [
                        NMatrix.new([6,1] , [
                                            -26468760.60299247,
                                            132758515.52645776,
                                             57556706.30607039,
                                            63115264.183926724,
                                                             0,
                                                             0
                                            ] ) ,
                        490.6703256499084
                       ]
                     }

      context "when epoch is a time string" do
        subject { test_body.state_at("2002", with_light_time: true) }
      
        it { is_expected.to ary_be_within(0.00001).of expected }
      end

      context "when epoch is seconds past J2000 Epoch" do
        subject { test_body.state_at(63115264.183926724, with_light_time: true) }
      
        it { is_expected.to ary_be_within(0.00001).of expected }
      end      
    end  
    
    context "When computing states of a body at multiple epochs in input array" do
      let(:expected) { }

      subject { test_body.state_at([63115264.183926724, "2003", "2004", "2005"]) }

      it { is_expected.to be_within(0.00001).of expected } 
    end
  end      

  describe "#velocity_at" do
    context "When all parameters are specified" do
      let(:expected) {  NMatrix.new([3,1], [ 91941265.18476546, 
                                             0, 
                                             0
                                           ] ) 
                     }
      
      subject { Body.new(:moon).velocity_at("2006 JAN 31 01:00", observer: :MARS, frame: :J2000, aberration_correction: :NONE) }
      
      it { is_expected.to be_within(0.00000001).of expected }
    end

    context "When computing velocity of a body at an epoch" do
      let(:expected) { [
                        NMatrix.new([3,1] , [
                                            63115264.183926724,
                                                             0,
                                                             0
                                            ] ) ,
                        490.6703256499084
                       ]
                     }

      context "when epoch is a time string" do
        subject { test_body.velocity_at("2002", with_light_time: true) }
      
        it { is_expected.to ary_be_within(0.00001).of expected }
      end

      context "when epoch is seconds past J2000 Epoch" do
        subject { test_body.velocity_at(63115264.183926724, with_light_time: true) }
      
        it { is_expected.to ary_be_within(0.00001).of expected }
      end
    end  
    
    context "When computing velocities of a body at multiple epochs in input array" do
      let(:expected) { }

      subject { test_body.velocity_at([63115264.183926724, "2003", "2004", "2005"]) }

      it { is_expected.to be_within(0.00001).of expected } 
    end
  end

  describe("#within_proximity") do
    context "When checking if bodies are within a certain radial distance" do
      let(:expected) { }

      subject { test_body.within_proximity(distance, epoch, [499, 599, :moon]) }

      it { is_expected.to eq expected }
    end
    
    context "When checking if any planets are within a certain radial distance" do
      let(:expected) { }

      subject { test_body.within_proximity(distance, epoch, [:planets]) }

      it { is_expected.to eq expected }
    end
  end

  describe("#distance_from") do
    context "when computing euclidean distance between the Moon and the Earth" do
      let(:expected) {}
      
      subject { test_body.distance_from(Body.new(:moon), epoch) }

      it { is_expected.to be eq expected }
    end
    
    context "When checking distance between a body and itself" do

      subject { test_body.within_proximity(Body.new(:earth), epoch) }

      it { is_expected.to be eq 0 }
    end
  end
end