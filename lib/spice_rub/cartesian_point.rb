module SpiceRub
  #
  #
  class CartesianPoint < BasePoint
    #
    # Can make this cleaner by using the splat operator
    #  working on getting it right
    def initialize(components, type = :rectangular)
      raise "Invalid type" unless [:rec, :pgr, :lat, :sph, :rad, :rectangular, :spherical, :latitudinal, :planetographic].include? type
      @ctype = type
      
      super(components)
    end

    # Transformation is basically matrix multiplication [3,3] x [3,1] -> [3,1]
    # where the [3,3] matrix is the frame transformation output of pxform
    def transform_frame(transformation_matrix)
      transformation_matrix.dot(self)    
    end
    

    def to_planetographic(body, equatorial_radius, flattening_coefficient)
      SpiceRub::Native.recpgr(body, self, equatorial_radius, flattening_coefficient)
    end
    alias :to_pgr :to_planetographic

    #def to_rec
    #  raise "already in rectangular co-ordinates"
    #end

    def to_spherical
      SpiceRub::Native.recsph(self)    
    end
    alias :to_sph :to_spherical

    def to_latitudinal
      SpiceRub::Native.reclat(self)
    end
    alias :to_lat :to_latitudinal
    
    def to_orbital
      SpiceRub::Native.recrad(self)
    end
    alias :to_rad :to_orbital  
  
  end
end