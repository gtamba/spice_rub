#--
# = SpiceRub
#
# A wrapper to the SPICE TOOLKIT for space and astronomomical
# computation in Ruby.
#
#
# == body.rb
#
# Contains the Body class, which is the Ruby interface to 
# Ephemeris calculation functions such as spkezr_c , sxform_c, etc..
#
#++

module SpiceRub
  class Body
    
    attr_accessor :frame 
    attr_reader :code , :name, :type

    def initialize(name , frame: :J2000)

      if name.is_a? Integer
        @name = SpiceRub::Native.bodc2n(name).downcase
        @code = name
      elsif name.is_a? Symbol
        @name = name.downcase
        @code = SpiceRub::Native.bodn2c(name)
      else
        raise(ArgumentError, "argument must be a NAIF Integer ID or a key symbol")
      end  	
      
      @type = body_type(@code)
      @frame = frame
      #raise(SpiceError, "body not found in SPICE Data") if !(@code and @name)
    end
    
    def position_at(time, observer: :sun, frame: @frame, aberration_correction: nil, with_light_time: nil)
      #
      observer = observer.name if observer.is_a? SpiceRub::Body
      aberration_correction = :none unless aberration_correction

      case
      when time.is_a?(SpiceRub::Time)
        output = SpiceRub::Native.spkpos(self.name, time.to_et, frame, aberration_correction, observer)
        with_light_time ? output : output[0]
      when time.is_a?(Integer) 
        5
      end              
      
    end

    def body_type(body_id)
      case
        when body_id == 10
          return :star
        when /[1-9]99/ === body_id.to_s
          return :planet
        #
        # etc.
        else 
          return  nil
      end  
    end
  end
end