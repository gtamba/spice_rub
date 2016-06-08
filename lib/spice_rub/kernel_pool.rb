#--
# = SpiceRub
#
# A wrapper to the SPICE TOOLKIT for space and astronomomical
# computation in Ruby.
#
#
# == kernel_pool.rb
#
# Contains the KernelPool class, SpiceKernel(nested within KernelPool) which is
# the Ruby interface to kernel loading functions such as
# furnsh_c , unload_c, ktotal_c, kclear_c
#++

require 'spice_rub.so'
require 'singleton'

module SpiceRub
  # KernelPool class, the Ruby interface to kernel loading functions such as
  # furnsh_c , unload_c, ktotal_c, kclear_c
  class KernelPool
    include Singleton

    attr_reader :pool
    attr_accessor :path

    def [](kernel)
      @pool[kernel]
    end

    def loaded
      @pool.select(&:loaded?)
    end

    def load(file)
      @pool ||= []

      file = @path.dup << file if check_path
      @pool << SpiceKernel.new(file) if SpiceRub::Native.furnsh(file)
      @pool.length - 1
    end

    def clear!
      unless empty?
        if SpiceRub::Native.kclear
          @pool = []
          return true
        end
      end
      false
    end

    def count(category = :ALL)
      SpiceRub::Native.ktotal(category)
    end

    def empty?
      count.zero?
    end

    def check_path
      if @path && @path.is_a?(String)
        @path << '/' unless @path[-1] == '/'
        true
      else
        false
      end
    end

    def clear_path!
      @path = nil
    end

    private :check_path

    # SpiceKernel class, a helper object used by KernelPool to track
    # kernel files, is not visible outside KernelPool.
    class SpiceKernel
      attr_reader :path_to, :loaded

      def initialize(path)
        @path_to = path
        @loaded = true
      end

      def unload
        if SpiceRub::Native.unload(@path_to)
          @loaded = false
          true
        else
          false
        end
      end

      def loaded?
        @loaded
      end
    end
    private_constant :SpiceKernel
  end
end
