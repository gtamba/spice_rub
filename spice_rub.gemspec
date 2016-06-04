Gem::Specification.new do |gem|
  gem.name        = 'spice_rub'
  gem.version     = '0.0.0'
  gem.license    = 'BSD 3-clause'
  gem.summary     = "SpiceRub is a Ruby wrapper for NASA's CSPICE libray"
  gem.description = "SpiceRub is a Ruby wrapper for NASA's CSPICE libray used for various astronomical calculations using flight data stored in kernel files"
  gem.authors     = ["Gaurav Tamba"]
  gem.email       = 'gaurav.tamba@gmail.com'
  gem.homepage    = 'https://github.com/gau27/spice_rub'
  gem.extensions = ['ext/spice_rub/extconf.rb']
  gem.require_paths = ["lib"]
  gem.files         = `git ls-files -- ext/spice_rub`.split("\n")
  gem.files        += `git ls-files -- lib`.split("\n")
  #Temp till submodule gets in
  gem.files        += ["ext/spice_rub/cspice/lib/libcspice.a"]
  gem.test_files    = `git ls-files -- spec`.split("\n")
  gem.required_ruby_version = '>= 1.9'

  gem.add_development_dependency 'bundler', '~>1.6'
  gem.add_development_dependency 'rake', '~>10.3'
  gem.add_development_dependency 'rake-compiler', '~>0.8'
  gem.add_development_dependency 'rspec', '~>2.14'
  gem.add_development_dependency 'pry', '~>0.10'
  gem.add_development_dependency 'rubocop', '>= 0.40.0'
  gem.add_development_dependency 'nmatrix', '>= 0.2.1'


end
