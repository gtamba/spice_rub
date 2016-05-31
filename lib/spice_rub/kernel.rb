require 'spice_rub'
require 'singleton'

module SpiceRub
	class KernelPool
	  include Singleton
	  	
	  attr_reader :kernel_pool 
	  attr_accessor :path

	  def load(file)
	  	kernel_pool |= []
	  	# should be Kernel.new

	  	if path
	  	  check_path
	  	  file = path.dup << file
	    end

	    SpiceRub::furnsh(file)
	  end

	  def unload!(file)
	    if path
	  	  check_path
	  	  file = path.dup << file
	    end

	  	SpiceRub::unload(file)
	  end

	  def count(category=:ALL)
	  	SpiceRub::ktotal(category)
	  end

	  def empty?
	  	count == 0 ? true : false
	  end

	  def check_path
	    #TODO, Add platform independency or remove this (For Windows filesystem)
	    if path and path.is_a? String
	    	path << '/' unless path[-1] == '/'
	    end	
	  end

	  def clear_path!
	  	@path = nil
	  end
	  
	  #Kernel Class in under construction	
	  class Kernel	
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
  	  private_constant :Kernel
  	end	
end