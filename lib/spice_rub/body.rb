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
    #default frame of reference for SPK related calculations
    attr_accessor :frame 
    
    #Body metadata as referred in
    attr_reader :code , :name, :type

    def initialize(name , frame: :J2000)
      case name
      when Integer
        search = Native.bodc2n(name)
        @name = search.downcase if search
        @code = name
      when Symbol
        @name = name.downcase
        search = Native.bodn2c(name)
        @code = search if search
      else
        raise(ArgumentError, "argument must be a NAIF Integer ID or a key symbol")
      end  	

      raise(SpiceError, "body not found in SPICE data") if ( !@code or !@name )
      
      @type = body_type(@code)
      @frame = frame
    end
    
    def position_at(time, observer: :sun, frame: @frame, aberration_correction: nil, with_light_time: nil)
      raise(ArgumentError, "Expected instance of SpiceRub::Time") unless time.is_a? Time
      
      observer = observer.name if observer.is_a? Body
      aberration_correction = :none unless aberration_correction

      output = Native.spkpos(self.name, time.et, frame, aberration_correction, observer)
      with_light_time ? output : output[0] 
    end
    
    def positions_at(time, observer: :sun, frame: @frame, aberration_correction: nil, with_light_time: nil)
      raise(ArlgumentError, "Expected array of time epochs") unless time.is_a? Array
      raise(ArgumentError, "Expected instance(s) of SpiceRub::Time") unless 
        time.all? {|t| t.is_a? Time}      
      
      observer = observer.name if observer.is_a? Body
      aberration_correction = :none unless aberration_correction  
           
      time.map do |epoch| 
        position = Native.spkpos(self.name, epoch.et, frame, aberration_correction, observer)
        with_light_time ? position : position[0] 
      end
    end    

    def state_at(time, observer: :sun, frame: @frame, aberration_correction: nil, with_light_time: nil)
      raise(ArgumentError, "Expected instance of SpiceRub::Time") unless time.is_a? Time

      observer = observer.name if observer.is_a? Body
      aberration_correction = :none unless aberration_correction
      
      output = Native.spkezr(self.name, time.et, frame, aberration_correction, observer)
      with_light_time ? output : output[0]    
    end

    def states_at(time, observer: :sun, frame: @frame, aberration_correction: nil, with_light_time: nil)
      raise(ArgumentError, "Expected array of time epochs") unless time.is_a? Array
      raise(ArgumentError, "Expected instance(s) of SpiceRub::Time") unless 
        time.all? {|t| t.is_a? Time}

      observer = observer.name if observer.is_a? Body
      aberration_correction = :none unless aberration_correction
             
      time.map do |epoch|
        state = Native.spkezr(self.name, epoch.et, frame, aberration_correction, observer)
        with_light_time ? state : state[0]
      end      
    end 

    def velocity_at(time, observer: :sun, frame: @frame, aberration_correction: nil, with_light_time: nil)
      raise(ArgumentError, "Expected instance of SpiceRub::Time") unless time.is_a? Time

      observer = observer.name if observer.is_a? Body
      aberration_correction = :none unless aberration_correction
      
      output = Native.spkezr(self.name, time.et, frame, aberration_correction, observer)
      with_light_time ?  [output[0][3..5], output[1]] : output[0][3..5]
    end
    
    def velocities_at(time, observer: :sun, frame: @frame, aberration_correction: nil, with_light_time: nil)
      raise(ArgumentError, "Expected array of time epochs") unless time.is_a? Array
      raise(ArgumentError, "Expected instance(s) of SpiceRub::Time") unless 
        time.all? {|t| t.is_a? Time}

      observer = observer.name if observer.is_a? Body
      aberration_correction = :none unless aberration_correction
      
      time.map do |epoch|
        state = Native.spkezr(self.name, epoch.et, frame, aberration_correction, observer)
        with_light_time ? [state[0][3..5], state[1]] : state[0][3..5]  
      end
    end

    def light_time_from(target, time, frame: @frame, aberration_correction: nil)
      raise(ArgumentError, "Expected instance of SpiceRub::Time") unless time.is_a? Time 

      target = target.name if target.is_a? Body
      aberration_correction = :none unless aberration_correction
      
      Native.spkpos(target, time.et, frame, aberration_correction, self.name)[1]
    end

    def distance_from(target, time, frame: @frame, aberration_correction: nil)
      raise(ArgumentError, "Expected instance(s) of SpiceRub::Time") unless time.is_a? Time

      target = target.name if target.is_a? Body
      aberration_correction = :none unless aberration_correction
   
      position = Native.spkpos(target, time.et, frame, aberration_correction, self.name)[0]
      Math.sqrt( (position ** 2).sum[0] )      
    end

    def within_proximity(bodies, distance, time, frame: @frame, aberration_correction: nil)
      raise(ArgumentError, "Expected array of bodies") unless bodies.is_a? Array
      raise(ArgumentError, "Expected instance(s) of SpiceRub::Body") unless
        bodies.all? { |b| b.is_a? Body }
      
      bodies.reject do |b| 
        self.distance_from(b, time, frame: frame, aberration_correction: aberration_correction) > 
          distance
      end
    end

    def rotate_position(time, target)
      pxform(@frame, target, time)
    end

    def rotate_state(time, target)
      sxform(@frame, target, time)
    end

    def body_type(body_id)
      if body_id > 2000000
        :asteroid
      elsif body_id > 1000000
        :comet
      elsif body_id > 1000
        :body
      elsif body_id > 10
        body_id % 100 == 99 ?
          :planet : :satellite
      elsif body_id == 10
        :star
      elsif body_id > 0
        :barycenter
      elsif body_id > -1000
        :spacecraft
      elsif body_id > -100000
        :instrument
      else
        :spacecraft
      end
    end              
  end
end