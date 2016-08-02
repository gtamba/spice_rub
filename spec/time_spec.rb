
# == time_spec.rb
#
# Basic tests for Time class , a custom version of Time and DateTime
#
#

require 'spec_helper'
require 'date'

describe SpiceRub::Time do
  

  describe ".new" do
    context "When instantiating with a time string representing the year only" do
      subject { SpiceRub::Time.new("2002") }

      its(:year)   { is_expected.to be eq 2002 }
      its(:month)  { is_expected.to be eq 1    }
      its(:day)    { is_expected.to be eq 1    }
      its(:hour)   { is_expected.to be eq 0    }
      its(:minute) { is_expected.to be eq 0    }
      its(:second) { is_expected.to be eq 0    }
    end

    context "When instantiating with Year, Month, Day, Hour" do
      subject { SpiceRub::Time.new(year: 2002, month: 3, day: 3, hour: 0, minute: 0 , second: 0) }

      its(:year)   { is_expected.to be eq 2002 }
      its(:month)  { is_expected.to be eq 3    }
      its(:day)    { is_expected.to be eq 3    }
      its(:minute) { is_expected.to be eq 0    }
      its(:hour)   { is_expected.to be eq 0    }
      its(:second) { is_expected.to be eq 0    }
    end
  end

  describe ".from_date_time" do
    context "When creating from a DateTime object" do
      subject { SpiceRub::Time.from_date_time(DateTime.new("2004")) }

      its(:year)   { is_expected.to be eq 2004 }
      its(:month)  { is_expected.to be eq 1    }
      its(:day)    { is_expected.to be eq 1    }
      its(:minute) { is_expected.to be eq 0    }
      its(:hour)   { is_expected.to be eq 0    }
      its(:second) { is_expected.to be eq 0    }
    end  
  end

  describe ".from_time" do
    context "When creating from a Time object" do
      subject { SpiceRub::Time.from_time(Time.new("2005")) }

      its(:year)   { is_expected.to be eq 2005 }
      its(:month)  { is_expected.to be eq 1    }
      its(:day)    { is_expected.to be eq 1    }
      its(:minute) { is_expected.to be eq 0    }
      its(:hour)   { is_expected.to be eq 0    }
      its(:second) { is_expected.to be eq 0    }
    end  
  end

  describe ".from_ephemeris_time" do
    context "When creating from ephemeris time float" do
      subject { SpiceRub::Time.from_ephemeris_time(254145665.1844829) }

      its(:year)   { is_expected.to be eq 2008 }
      its(:month)  { is_expected.to be eq 1    }
      its(:day)    { is_expected.to be eq 21    }
      its(:minute) { is_expected.to be eq 0    }
      its(:hour)   { is_expected.to be eq 0    }
      its(:second) { is_expected.to be eq 0    }
    end  
  end

  describe ".from_posix" do
    context "When creating from POSIX time stamo" do
      subject { SpiceRub::Time.from_posix(1470139800) }

      its(:year)   { is_expected.to be eq 2016 }
      its(:month)  { is_expected.to be eq 8    }
      its(:day)    { is_expected.to be eq 2    }
      its(:minute) { is_expected.to be eq 10    }
      its(:hour)   { is_expected.to be eq 12    }
      its(:second) { is_expected.to be eq 0    }
    end  
  end

  describe ".now" do
    context "When creating at current time in UTC" do
      let(:expected) { Time.now }

      subject { SpiceRub::Time.now }
        
      its(:year)   { is_expected.to be eq expected.year  }
      its(:month)  { is_expected.to be eq expected.month }
      its(:day)    { is_expected.to be eq expected.day   }
    end 
  end
  
  describe "#to_ephemeris_time" do
    subject { SpiceRub::Time.new(year: 2011, month: 2, day: 2).to_ephemeris_time }
      
    it { is_expected.to be eq 349876866.1848078 }
  end
  
  describe "#to_date_time" do
    subject { SpiceRub::Time.new(year: 2010, month: 5, day: 15).to_date_time }
      
    it { is_expected.to be_instance_of(DateTime) }
  end

  describe "#to_string" do
  # Formatting?
  end
end 