require "mkmf"

$defs.push("-std=gnu99")
$defs.push("-Wall")
$defs.push("-Werror")

#This 
HEADER_DIRS = [File.expand_path(File.join(File.dirname(__FILE__), "cspice/include"))]
LIB_DIRS = [File.expand_path(File.join(File.dirname(__FILE__), "cspice/lib"))]

# array of all libraries that the C extension should be compiled against

dir_config('spicerub', HEADER_DIRS, LIB_DIRS)

$LOCAL_LIBS << "-lcspice"

find_header("SpiceUsr.h")
find_library("cspice", "furnsh_c")
find_library("cspice", "spkez_c")

create_makefile('/spicerub')