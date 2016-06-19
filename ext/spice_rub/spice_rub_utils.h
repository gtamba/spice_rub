const size_t VECTOR_SHAPE[2] = {1,3};
const size_t STATE_SHAPE[2] = {1,6};
//Macros for switch parameters in error message reports
#define SPICE_ERROR_SHORT 0
#define SPICE_ERROR_LONG 1
#define SPICE_ERROR_EXPLAIN 2
#define SPICE_ERROR_ALL 3

//Macros for simplifying frequent operations
// Converts a Ruby Symbol to C string
#define RB_SYM2STR(val) (rb_id2name(SYM2ID(val)))
#define RB_STR2SYM(val) (ID2SYM(rb_intern(val)))