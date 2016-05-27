require "mkmf"

$defs.push("-std=gnu99")
$defs.push("-Wall")
$defs.push("-Werror")


LIBDIR     = RbConfig::CONFIG['libdir']
INCLUDEDIR = RbConfig::CONFIG['includedir']

HEADER_DIRS = [INCLUDEDIR, File.expand_path(File.join(File.dirname(__FILE__), "cspice/include"))]

# setup constant that is equal to that of the file path that holds that static libraries that will need to be compiled against
LIB_DIRS = [LIBDIR, File.expand_path(File.join(File.dirname(__FILE__), "cspice/lib"))]

# array of all libraries that the C extension should be compiled against

dir_config('spicerub', HEADER_DIRS, LIB_DIRS)

$LOCAL_LIBS << "-lcspice"


find_header("SpiceUsr.h")
find_library("cspice", "furnsh_c")


create_makefile('spicerub')