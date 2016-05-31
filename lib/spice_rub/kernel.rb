require 'spice_rub'

module SpiceRub
	class KernelPool

	  attr_reader :kernel_pool 
	
	  def load(file)
	    SpiceRub::furnsh(file)
	  end


	end
end