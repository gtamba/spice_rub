# -*- ruby -*-

require 'rubygems'
require 'rubygems/package_task'
require 'bundler'

# Use https://github.com/SciRuby/nmatrix/blob/master/Rakefile
# as an example. Note that the order of requires is somewhat important, or was in the past.
# You also will only need one gemspec, so you can probably simplify some of the code.

gemspec = eval(IO.read("spice_rub.gemspec"))


begin
  Bundler.setup(:default, :development)
rescue Bundler::BundlerError => e
  $stderr.puts e.message
  $stderr.puts "Run `bundle install` to install missing gems"
  exit e.status_code
end

desc "Build and install into system gems."
task :install => :repackage do
  gem_file = "pkg/#{gemspec.name}-#{gemspec.version}.gem"
  system "gem install '#{gem_file}'"
end

require 'rake'
require 'rake/extensiontask'

gemspec.extensions.each do |extconf|
  ext_name = extconf.match(/ext\/(.*)\/extconf\.rb/)[1]
  Rake::ExtensionTask.new do |ext|
    ext.name = ext_name
    ext.ext_dir = "ext/#{ext_name}"
    ext.lib_dir = "lib/"
    ext.source_pattern = "**/*.{c,cpp,h}"
  end
end

Gem::PackageTask.new(gemspec).define

require 'rspec/core/rake_task'
require 'rspec/core'


task :spec => "spec:all"

task :console do |task|
  cmd = [ 'irb', "-r './lib/spice_rub.rb'" ]
  run *cmd
end

task :pry do |task|
  cmd = [ 'pry', "-r './lib/spice_rub.rb'" ]
  run *cmd
end

task :default => :spec

def run *cmd
  sh(cmd.join(" "))
end


