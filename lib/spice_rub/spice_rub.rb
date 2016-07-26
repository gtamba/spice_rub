#Load NMatrix dependency
require 'nmatrix'

# spice_rub.so ends up in either lib/ or lib/spice_rub  during development and gem install respectively
if File.exist?("lib/spice_rub/spice_rub.so")
  # Gem
  require "spice_rub/spice_rub.so"
else
  # Development
  require "spice_rub.so"
end

#Load all Ruby classes
require_relative './kernel_pool.rb'
require_relative './base_point.rb'
require_relative './cartesian_point.rb'
require_relative './body.rb'


