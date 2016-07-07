module SpiceRub
  #
  #
  class Point < NMatrix
    attr_reader :ctype
    
    #
    # Can make this cleaner by using the splat operator
    #  working on getting it right
    def initialize(type, payload)
      raise "Invalid type" unless [:rec, :pgr, :lat, :sph, :rad].include? type
      @ctype = type
      super([payload.length,1], payload , dtype: :float64)
    end

    # Transformation is basically matrix multiplication [3,1] x [3,3] -> [3,1]
    # where the [3,3] matrix is the frame transformation output of pxform
    def transform_frame(tranformation_matrix)
      # If we're already in rectangular, go ahead and transform
      #
      if @ctype == :rec
        self.dot(tranformation_matrix)
      # If not, convert to rectangular, transform and then
      #  ? convert back to original ? (Still havent figured it out completely)
      else
        #Convert and transform
        transformed = self.to_rec.dot(tranformation_matrix)
        #Restore to original co-ordinate type
        # Probably will make transformed a Point if all the conversion functions can be worked out
        SpiceRub::Native.send("rec" << @ctype.to_s, transformed)
      end     
    end

    def to_pgr
      raise "already in planetographic co-ordinates" if @ctype == :pgr
      #Probably the most complicated one to deal with      

      #Todo
    end

    def to_rec
      raise "already in rectangular co-ordinates" if @ctype == :rec
      # This will not work for pgr because some additional parameters are required
      SpiceRub::Native.send(@ctype.to_s << "rec", self[0], self[1], self[2])
    end

    def to_sph
      raise "already in spherical co-ordinates" if @ctype == :sph
      SpiceRub::Native.send(@ctype.to_s << "sph", self)    
    end

    def to_lat
      raise "already in latitudinal co-ordinates" if @ctype == :lat
      #Todo
    end

    def to_rad
      raise "already in range, right ascension and declination" if @ctype == :rad
      #Todo
    end
      
  end
end