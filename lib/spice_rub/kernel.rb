require 'spice_rub.so'
require 'singleton'

module SpiceRub
  class KernelPool
    include Singleton

    attr_reader :pool 
    attr_accessor :path
    
    def [] kernel
      @pool[kernel]
    end

    def load(file)
      @pool ||= []
      # should be Kernel.new
      file = @path.dup << file if check_path
      @pool << SpiceKernel.new(file) if SpiceRub::furnsh(file)
      @pool.length - 1
    end

#    def unload!(file)
#      file = @path.dup << file if check_path
#      SpiceRub::unload(file)
#    end
    
    def clear!
      SpiceRub::kclear unless self.empty?
    end
      
    def count(category=:ALL)
      SpiceRub::ktotal(category)
    end

    def empty?
      count.zero?
    end

    def check_path
      if @path and @path.is_a? String
        @path << '/' unless @path[-1] == '/'
        return true
      end	
      
      false
    end

    def clear_path!
      @path = nil
    end
	  
    #SpiceKernel Class is under construction	
    class SpiceKernel
      attr_reader :path_to , :loaded
      
      def initialize(path)
        @path_to = path
        @loaded = true
      end
      
      def unload
        if SpiceRub::unload(@path_to) 
          @loaded = false
          return true
        else
          return false
        end
      end
      
      def loaded?
      	@loaded
      end
    end
    private_constant :SpiceKernel
  end	
end
