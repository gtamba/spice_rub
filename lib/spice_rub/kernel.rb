require 'spice_rub.so'
require 'singleton'

module SpiceRub
  class KernelPool
    include Singleton

    attr_reader :kernel_pool 
    attr_accessor :path

    def load(file)
      kernel_pool ||= []
      # should be Kernel.new
      file = @path.dup << file if check_path
      SpiceRub::furnsh(file)
    end

    def unload!(file)
      file = @path.dup << file if check_path
      SpiceRub::unload(file)
    end
    
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
      end
      
      def loaded?
      	@loaded
      end
    end
    private_constant :SpiceKernel
  end	
end
