module SpiceRub
  class BasePoint < NMatrix
    attr_reader :ctype

    def initialize(components)
      super([components.length, 1], components, dtype: :float64)
    end  
  end
  private_constant :BasePoint
end