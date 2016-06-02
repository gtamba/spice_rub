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
      if @path
        check_path
        file = @path.dup << file
      end

      SpiceRub::furnsh(file)
    end

    def unload!(file)
      if @path
        check_path
	    file = @path.dup << file
	  end

	  SpiceRub::unload(file)
    end

    def count(category=:ALL)
      SpiceRub::ktotal(category)
    end

    def empty?
      count.zero?
    end

    def check_path
      #TODO, Add platform independence or remove this (For Windows filesystem)
	  if @path and @path.is_a? String
	    @path << '/' unless @path[-1] == '/'
	  end	
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
	      #TODO
	    end
	  	
	    def loaded?
	     @loaded
	    end
    end
  	private_constant :SpiceKernel
  end	
end