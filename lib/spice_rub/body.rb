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
      case name
      when Integer
        @name = Native.bodc2n(name).downcase
        @code = name
      when Symbol
        @name = name.downcase
        @code = Native.bodn2c(name)
      else
        raise(ArgumentError, "argument must be a NAIF Integer ID or a key symbol")
      end  	
      
      @type = body_type(@code)
      @frame = frame
      raise(SpiceError, "body not found in SPICE Data") if ( !@code and !@name )
    end
    
    def position_at(time, observer: :sun, frame: @frame, aberration_correction: nil, with_light_time: nil)
      #
      observer = observer.name if observer.is_a? Body
      aberration_correction = :none unless aberration_correction
      
      case time
      when Time 
        output = Native.spkpos(self.name, time.et, frame, aberration_correction, observer)
        with_light_time ? output : output[0]
      when Array 
        raise(ArgumentError, "Expected instance(s) of SpiceRub::Time") unless time.all? { |t| t.is_a? Time } 
        
        output = []

        time.each do |epoch| 
          position = Native.spkpos(self.name, epoch.et, frame, aberration_correction, observer)
          output << ( with_light_time ? position : position[0] ) 
        end
        output
      else
        raise(ArgumentError, "Expected instance(s) of SpiceRub::Time") 
      end      
    end
    
    def state_at(time, observer: :sun, frame: @frame, aberration_correction: nil, with_light_time: nil)
      #
      observer = observer.name if observer.is_a? Body
      aberration_correction = :none unless aberration_correction
      
      case time
      when Time
        output = Native.spkezr(self.name, time.et, frame, aberration_correction, observer)
        with_light_time ? output : output[0]
      when Array
        raise(ArgumentError, "Expected instance(s) of SpiceRub::Time") unless time.all? { |t| t.is_a? Time } 
        
        output = []

        time.each do |epoch|
          state = Native.spkezr(self.name, epoch.et, frame, aberration_correction, observer)
          output << ( with_light_time ? state : state[0] )
        end
        output
      else
        raise(ArgumentError, "Expected instance(s) of SpiceRub::Time") 
      end      
    end
    

    def velocity_at(time, observer: :sun, frame: @frame, aberration_correction: nil, with_light_time: nil)
      #
      observer = observer.name if observer.is_a? Body
      aberration_correction = :none unless aberration_correction
      
      case time
      when Time
        output = Native.spkezr(self.name, time.et, frame, aberration_correction, observer)
        with_light_time ?  [output[0][0..2], output[1]] : output[0][0..2]
      when Array
        raise(ArgumentError, "Expected instance(s) of SpiceRub::Time") unless time.all? { |t| t.is_a? Time } 
        
        output = []

        time.each do |epoch|
          state = Native.spkezr(self.name, epoch.et, frame, aberration_correction, observer)
          output << ( with_light_time ? [state[0][3..5], state[1]] : state[0][3..5] )
        end
        output
      else
        raise(ArgumentError, "Expected instance(s) of SpiceRub::Time") 
      end      
    end

    def light_time_from(target, time, frame: @frame, aberration_correction: nil)
      raise(ArgumentError, "Expected instance of SpiceRub::Time") unless time.is_a? Time 

      target = target.name if target.is_a? Body
      aberration_correction = :none unless aberration_correction
      
      Native.spkpos(target, time.et, frame, aberration_correction, self.name)[1]
    end

    def distance_from(target, time, frame: @frame, aberration_correction: nil)
      raise(ArgumentError, "Expected instance of SpiceRub::Time") unless time.is_a? Time 

      target = target.name if target.is_a? Body
      aberration_correction = :none unless aberration_correction
      
      position = Native.spkpos(target, time.et, frame, aberration_correction, self.name)[0]
      Math.sqrt( (position ** 2).sum[0] )      
    end

    def within_proximity(body, distance, time, frame: @frame, aberration_correction: nil)
      case body
      when Body
        body.distance_from(self) < distance ? [body] : nil 
      when Array
        ##todo
     else
     end
    end
    
    def body_type(body_id)
      case
        when body_id == 10
          return :star
        when /[1-9]99/ === body_id.to_s
          return :planet
        when /[1-9]/ === body_id.to_s
          return :barycenter
        #
        # etc.
        else 
          return  nil
      end  
    end
  end
end