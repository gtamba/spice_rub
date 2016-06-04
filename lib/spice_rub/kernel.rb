require 'spice_rub.so'
require 'singleton'

module SpiceRub
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
      # should be Kernel.new
      file = @path.dup << file if check_path
      @pool << SpiceKernel.new(file) if SpiceRub.furnsh(file)
      @pool.length - 1
    end

    def clear!
      unless empty?
        if SpiceRub.kclear
          @pool = []
          return true
        end
      end
      false
    end

    def count(category = :ALL)
      SpiceRub.ktotal(category)
    end

    def empty?
      count.zero?
    end

    def check_path
      if @path and @path.is_a? String
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
    # SpiceKernel Class is under construction
    class SpiceKernel
      attr_reader :path_to, :loaded

      def initialize(path)
        @path_to = path
        @loaded = true
      end

      def unload
        if SpiceRub.unload(@path_to)
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
