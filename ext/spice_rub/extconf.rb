require "mkmf"

#This is a temporary extconf.rb file that assumes that I've moved cspice.a to /ext/spice_rub/cspice/lib/libspice.a
# TODO : Make a script that works together with extconf.rb to use a local installation or to fetch one from NAIF's FTP
HEADER_DIRS = [File.expand_path(File.join(File.dirname(__FILE__), "cspice/include"))]
LIB_DIRS = [File.expand_path(File.join(File.dirname(__FILE__), "cspice/lib"))]

dir_config('spice_rub', HEADER_DIRS, LIB_DIRS)

$LOCAL_LIBS << "-lcspice"

abort "Cannot locate necessary header files : SpiceUsr.h" unless find_header("SpiceUsr.h")
abort "C SPICE Library is not exposed (Incorrect Configuration/Invalid installation)" unless find_library("cspice", "furnsh_c")
abort "C SPICE Library is not exposed (Incorrect Configuration/Invalid installation)" unless find_library("cspice", "spkez_c")

$defs.push("-std=gnu99")
#$defs.push("-Wall")
#$defs.push("-Werror")

create_makefile('spice_rub/spice_rub')