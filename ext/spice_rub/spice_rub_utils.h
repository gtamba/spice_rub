//Forward Declarations of Utility Functions
bool spice_error(int error_detail);
sigset_t block_signals();
void restore_signals(sigset_t old_mask);

//Macros for switch parameters in error message reports
#define SPICE_ERROR_SHORT 0
#define SPICE_ERROR_LONG 1
#define SPICE_ERROR_EXPLAIN 2
#define SPICE_ERROR_ALL 3

//Macros for simplifying frequent operations
// Converts a Ruby Symbol to C string
#define RB_SYM2STR(val) (rb_id2name(SYM2ID(val)))
#define RB_STR2SYM(val) (ID2SYM(rb_intern(val)))