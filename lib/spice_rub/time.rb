#--
# = SpiceRub
#
# A wrapper to the SPICE TOOLKIT for space and astronomomical
# computation in Ruby.
#
#
# == time.rb
#
# Contains the Time class, which is a wrapper class which always references
# Ephemeris Time (TDB seconds past J2000 Epoch) but has numerous class methods
# to construct from different time systems.
# 
# Requires a loaded leap second kernel.
#
#++

module SpiceRub
  class Time
    
    attr_reader :et
    alias :to_ephemeris_time :et
    alias :to_et :et
    
    @@system = :utc

    def initialize(epoch, seconds: :tdb)
      case seconds
      when :tdb
        @et = epoch
      when :utc
        deltet = Native.deltet(epoch, :utc)
        #ET = UTC + DELTET
        @et = epoch + deltet   
      when :tai, :tdt, :jdtdt, :tdb, :jed, :jdtdb
        @et = Native.unitim(epoch, seconds, :et)
      end
    end
    
    def self.at(offset, reference = :j2000, seconds: :tdb)  
      raise(ArgumentError, "Non-TDB seconds must reference J2000 Epoch") if 
        (![:utc, :tdb].include?(seconds) and 
      	  reference.downcase != :j2000)

      case reference.downcase
      when :j2100
        new(offset + 3155760000.0, seconds: seconds)
      when :j2000, :et
        new(offset, seconds: seconds)      
      when :j1950
        new(offset - 1577880000.0, seconds: seconds)
      when :j1900
        new(offset - 3155760000.0, seconds: seconds)
      when :gps
        new(offset - 630763148.8159368, seconds: seconds)
      when :unix
        new(offset - 946727958.8160644, seconds: seconds)
      end
    end
    
    def self.from_spacecraft_clock(spacecraft_id, time, encoding: :string)
      case encoding
      when :string
        raise(ArgumentError, "expected spacecraft clock string") unless time.is_a? String
        raise(ArgumentError, "expected integer spacecraft ID") unless spacecraft_id.is_a? Integer   
        new(Native.scs2e(spacecraft_id, time))
      when :ticks
        raise(ArgumentError, "expected float encoded spacecraft time (ticks)") unless time.is_a? Float
        raise(ArgumentError, "expected integer spacecraft ID") unless spacecraft_id.is_a? Integer   
        new(Native.sct2e(spacecraft_id, time))
      else
        raise(ArgumentError, "encoding must be :string or :ticks")
      end 
    end
    #alias :from_sc :from_spacecraft_clock

    def self.parse(string)
      new(Native.str2et(string))    
    end

    def self.from_time(time)
      warn "Default time system is not UTC, Ruby's Time class only supports UTC and its offsets" unless @@system == :utc
      new(Native.str2et(time.utc.to_s))
    end
    
    def self.from_tuple(*params)
      order = [:year, :month, :day, :hour, :min, :sec]
	    components = [nil, 1, 1, 0, 0, 0]
	
	    components = order.zip(components, params)
        .map { |sym, default, val| [sym, val || default] }.to_h
	    
      new(Native.str2et('%<month>02i/%<day>02i/%<year>04i %<hour>02i:%<min>02i:%<sec>02i' % 
        components))    
    end

    def self.now
      et = Native.str2et(::Time.now.utc.to_s)
      new(et)
    end
    
    def to_tai
      Native.unitim(@et, :et, :tai)
    end
    
    def to_tdt
      Native.unitim(@et, :et, :tdt)
    end

    def to_jdtdt
      Native.unitim(@et, :et, :jdtdt)
    end
    
    def to_tdb
      Native.unitim(@et, :et, :tdb)
    end
    
    def to_jed
      Native.unitim(@et, :et, :jed)
    end
    
    def to_jdtdb
      Native.unitim(@et, :et, :jdtdb)
    end

    def to_utc
      @et - Native.deltet(@et, :et)
    end

    def to_string(format = "Wkd Mon DD HR:MN:SC UTC YYYY ::UTC")
      timout(@et, format, 32)
    end
    alias :to_s :to_string


    def to_spacecraft_clock(spacecraft_id, encoding: :string)
      raise(ArgumentError, "expected integer spacecraft ID") unless spacecraft_id.is_a? Integer   
      ticks = sce2c(spacecraft_id, @et)  
      
      case encoding
      when :ticks
        ticks
      when :string
        scdecd(spacecraft_id, ticks, 32)
      else
        raise(ArgumentError, "encoding must be :string or :ticks")
      end
    end
    alias :to_sc :to_spacecraft_clock
  end
end