module SpiceRub
  class Time
    
    attr_reader :et
    alias :to_ephemeris_time :et
    alias :to_et :et
    
    @@system = :utc

    def initialize(*params)
      if params.empty?
        @et = Native.str2et(Object::Time.now.utc.to_s)      
      elsif params.size == 2 and params[1] == :et
        @et = params[0]
      else      
        year  = params[0]      
        month = params[1] || 1
        day   = params[2] || 1
        hour  = params[3] || 0
        min   = params[4] || 0
        sec   = params[5] || 0 
        
        @et = Native.str2et("#{month}/#{day}/#{year} #{hour}:#{min}#{sec}")
      end
    end
    
    def self.at(offset, scale = :j2000)  
      case scales.downcase
      when :j2100
        new(offset + 3155760000.0, :et)
      when :j2000, :et
        new(offset, :et)      
      when :j1950
        new(offset - 1577880000.0, :et)
      when :j1900
        new(offset - 3155760000.0, :et)
      when :tai, :tdt, :jdtdt, :tdb, :jed, :jdtdb
        et = Native.unitim(offset, scale, :et)
        new(et, :et)
      when :unix
        et = Native.str2et(Object::Time.at(offset).utc.to_s)
        new(et, :et)
      end
    end

    def self.parse(string)
      new(Native.str2et(string), :et)    
    end

    def self.from_time(time)
      warn "Default time system is not UTC, Ruby's Time class only supports UTC and its offsets" unless @@system == :utc
      new(Native.str2et(time.utc.to_s), :et)
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

    def utc_to_tdb(epoch , reference)
      #todo
    end
  end
end