#include "ruby.h"
#include "SpiceUsr.h"
#include <stdbool.h>
#include "spice_rub_utils.h"
#include "nmatrix.h"

const size_t EPHEM_POSITION_SHAPE[2] = {3,1};
const size_t EPHEM_STATE_SHAPE[2] = {6,1};