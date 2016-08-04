module SpiceRub
  class Time
    
    attr_accessor :et

    def self.from_string(str)
      lol = Time.new()
      lol.et = SpiceRub::Native.str2et(str)  
      lol
    end
  
    def to_ephemeris_time
      @et
    end 
    alias :to_et :to_ephemeris_time
  end
end