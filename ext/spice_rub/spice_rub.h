//Top level IMPLICIT Declarations

VALUE spicerub_module;
VALUE rb_spice_error;

//Kernel Loading Functions
static VALUE furnsh(VALUE self, VALUE kernel);
static VALUE unload(VALUE self, VALUE kernel);
static VALUE ktotal(int argc, VALUE *argv, VALUE self);
static VALUE kclear(VALUE self);

//Geometry and Co-ordinate System Function
static VALUE latrec(VALUE self, VALUE radius, VALUE longtitude, VALUE latitude, VALUE result);
static VALUE  lspcn(int argc, VALUE *argv, VALUE self);
static VALUE reclat(VALUE self, VALUE rectangular_point);
static VALUE sincpt(VALUE self, VALUE method, VALUE target, VALUE et, VALUE fixref, VALUE abcorr, VALUE obsrvr, VALUE dref, VALUE dvec);
static VALUE subpnt(VALUE self, VALUE method, VALUE target, VALUE et, VALUE fixref, VALUE abcorr, VALUE obsrvr);
static VALUE subslr(VALUE self, VALUE method, VALUE target, VALUE et, VALUE fixref, VALUE abcorr, VALUE obsrvr);
static VALUE recsph(VALUE self, VALUE rectangular);

//Time and Time Conversions Functions
static VALUE str2et(VALUE self, VALUE epoch);


//Helper Macros

//Macros for switch parameters in error message reports
#define SPICE_ERROR_SHORT 0
#define SPICE_ERROR_LONG 1
#define SPICE_ERROR_EXPLAIN 2
#define SPICE_ERROR_ALL 3

//Macros for simplifying frequent operations
// Converts a Ruby Symbol to C string
#define RB_SYM2STR(val) (rb_id2name(SYM2ID(val)))
#define RB_STR2SYM(val) (ID2SYM(rb_intern(val)))