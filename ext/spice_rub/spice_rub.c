#include "ruby.h"
#include "SpiceUsr.h"
#include "signal.h"
#include <stdbool.h>
#include "spice_rub.h"
#include "nmatrix.h"

/* This is a thread safety mechanism. CSPICE uses various unix signals and it is prudent to block them while 
 kernels are being loaded to ensure two threads do not interfere. This was inspired by similar blocks in place
 in the naif-spice gem : http://lunaserv.lroc.asu.edu/naif-spice.html

 Template usage :
 
 sigset_t old_mask = block_signals();

 #DO THREAD UNSAFE C SPICE CALLS

 restore_signals(old_mask);


  #TODO : Elaborate explanation	
*/

sigset_t block_signals() {
  sigset_t full_mask;
  sigset_t old_mask;
  
  //This UNIX routine sets all the mask bits to 1, effectively blocking every signal
  // indicated by the bitmask when sigprocmask is called
  sigfillset(&full_mask);
  
  //This UNIX routine utilizes the given mask to block the appropriate signals
  // The previous mask is stored in old_smask
  sigprocmask(SIG_BLOCK, &full_mask, &old_mask);

  return old_mask;
}

void restore_signals(sigset_t old_mask) {
  //Uses the parameter mask to restore signals to previous state
  sigprocmask(SIG_SETMASK, &old_mask, NULL);
}

bool spice_error(int error_detail) {
  SpiceInt buffer_size = 1024;
  char error_message[buffer_size];

  if (failed_c()) {
    
    switch(error_detail) {
      
      case SPICE_ERROR_ALL :
        //TODO: Neat way to concat all error messages.
        reset_c();
        break;

      case SPICE_ERROR_SHORT :
        getmsg_c("SHORT", buffer_size, error_message);
        reset_c();
        rb_raise(rb_spice_error, "%s\n", error_message);
        break;

      case SPICE_ERROR_LONG :
        getmsg_c("LONG", buffer_size, error_message);
        reset_c();
        rb_raise(rb_spice_error, "%s\n", error_message);
        break;

      case SPICE_ERROR_EXPLAIN :
        getmsg_c("EXPLAIN", buffer_size, error_message);
        reset_c();
        rb_raise(rb_spice_error, "%s\n", error_message);
        break;

      default :
        reset_c();
        break;
    }
  }
  
  return false;
}

static VALUE furnsh(VALUE self, VALUE kernel) {
  sigset_t old_mask = block_signals();

  furnsh_c(StringValuePtr(kernel));

  restore_signals(old_mask);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qfalse;

  return Qtrue;
}

static VALUE unload(VALUE self, VALUE kernel) {
  sigset_t old_mask = block_signals();

  unload_c(StringValuePtr(kernel));
  
  restore_signals(old_mask);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qfalse;

  return Qtrue;
}

static VALUE ktotal(int argc, VALUE *argv, VALUE self) {
  SpiceInt kernel_count;
  
  if(argc == 0) ktotal_c("ALL", &kernel_count);

  //Else convert Symbol to ID, ID to string if category argument supplied
  else ktotal_c(RB_SYM2STR(argv[0]), &kernel_count);

  spice_error(SPICE_ERROR_SHORT);

  return INT2FIX(kernel_count);
}

static VALUE kclear(VALUE self) {
  
  kclear_c();
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qfalse;

  return Qtrue;
}

//End of Kernel Functions

//Geometry and Co-ordinate Routines

static VALUE latrec(VALUE self, VALUE radius, VALUE longitude, VALUE latitude, VALUE result) {

  latrec_c(NUM2DBL(radius), NUM2DBL(longitude), NUM2DBL(latitude),  NM_STORAGE_DENSE(result)->elements);
  return result; 
}

static VALUE reclat(VALUE self, VALUE rectangular_point) {
  double radius, 
         longitude, 
         latitude;

  reclat_c(NM_STORAGE_DENSE(rectangular_point)->elements, &radius, &longitude, &latitude);
  
  return rb_ary_new3(3, DBL2NUM(radius), DBL2NUM(longitude), DBL2NUM(latitude));
}


//TODO : Test this, current kernel files do not cover this 
static VALUE lspcn(int argc, VALUE *argv, VALUE self) {
  double result;

  result = lspcn_c(StringValuePtr(argv[0]), NUM2DBL(argv[1]), StringValuePtr(argv[2]));

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  return DBL2NUM(result); 
}
/*

void sincpt_c :

Given an observer and a direction vector defining a ray, compute 
the surface intercept of the ray on a target body at a specified 
epoch, optionally corrected for light time and stellar 
aberration. 


void sincpt_c (    ConstSpiceChar      * method,
                   ConstSpiceChar      * target,
                   SpiceDouble           et,
                   ConstSpiceChar      * fixref,
                   ConstSpiceChar      * abcorr,
                   ConstSpiceChar      * obsrvr,
                   ConstSpiceChar      * dref,
                   ConstSpiceDouble      dvec   [3],
                   SpiceDouble           spoint [3],
                   SpiceDouble         * trgepc,
                   SpiceDouble           srfvec [3],
                   SpiceBoolean        * found       )

*/
static VALUE sincpt(VALUE self, VALUE method, VALUE target, VALUE et, VALUE fixref, VALUE abcorr, VALUE obsrvr, VALUE dref, VALUE dvec) {
  //C containers to pass on to SPICE function
  
  /* Input
   argv[0] -> Target
   argv[1] -> Ephemeris Time
   argv[2] -> Fixed Reference
   argv[3] -> Aberration Correcton *IGNORED* if Arguments passed is 	
   argv[4] -> Observer Body Name
   argv[5] -> Reference Frame of Ray's direction vector
   argv[6] -> Ray's direction Vector
  */
  
  double ray_direction_vector[3] = {0.0,0.0,0.0}; //
 
  //Output parameters
  SpiceBoolean found;
  double intercept_epoch,
         surface_point[3],
         surface_vector[3];

  //Arrays that we return to Ruby
  VALUE rb_vector = rb_ary_new();
  VALUE rb_point  = rb_ary_new();
  
  //Loop control
  int count;

  for(count = 0; count < 3; count++) {
  	//Should be nmatrix?
    ray_direction_vector[count] = NUM2DBL(RARRAY_PTR(dvec)[count]);
  }
  
  sincpt_c(StringValuePtr(method), StringValuePtr(target), NUM2DBL(et), StringValuePtr(fixref), StringValuePtr(abcorr), StringValuePtr(obsrvr), StringValuePtr(dref), ray_direction_vector, surface_point, &intercept_epoch, surface_vector, &found);

  if(!found) {
  	return Qfalse;
  }

  else if(spice_error(SPICE_ERROR_SHORT)) {
  	return Qnil;
  }

  for (count = 0; count < 3; count++) {
    rb_ary_push(rb_vector, DBL2NUM(surface_vector[count]));
    rb_ary_push(rb_point, DBL2NUM(surface_point[count]));
  }  

  return rb_ary_new3(3, rb_point, rb_vector, DBL2NUM(intercept_epoch));
}

/*

   void subpnt_c ( ConstSpiceChar       * method,
                   ConstSpiceChar       * target,
                   SpiceDouble            et,
                   ConstSpiceChar       * fixref,
                   ConstSpiceChar       * abcorr,
                   ConstSpiceChar       * obsrvr,
                   SpiceDouble            spoint [3],
                   SpiceDouble          * trgepc,
                   SpiceDouble            srfvec [3]

*/

static VALUE subpnt(VALUE self, VALUE method, VALUE target, VALUE et, VALUE fixref, VALUE abcorr, VALUE obsrvr) {
  //loop control
  int count;

  //Output
  double observer_epoch,
         surface_point[3],
         surface_vector[3];

  //Arrays that we return to Ruby
  VALUE rb_vector = rb_ary_new();
  VALUE rb_point  = rb_ary_new();

  subpnt_c(StringValuePtr(method), StringValuePtr(target), NUM2DBL(et), StringValuePtr(fixref), StringValuePtr(abcorr), StringValuePtr(obsrvr), surface_point, &observer_epoch, surface_vector);

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  for (count = 0; count < 3; count++) {
    rb_ary_push(rb_vector, DBL2NUM(surface_vector[count]));
    rb_ary_push(rb_point, DBL2NUM(surface_point[count]));
  }  

  return rb_ary_new3(3, rb_point, rb_vector, DBL2NUM(observer_epoch));
}

/*

   void subslr_c ( ConstSpiceChar       * method,
                   ConstSpiceChar       * target,
                   SpiceDouble            et,
                   ConstSpiceChar       * fixref,
                   ConstSpiceChar       * abcorr,
                   ConstSpiceChar       * obsrvr,
                   SpiceDouble            spoint [3],
                   SpiceDouble          * trgepc,
                   SpiceDouble            srfvec [3]

*/

static VALUE subslr(VALUE self, VALUE method, VALUE target, VALUE et, VALUE fixref, VALUE abcorr, VALUE obsrvr) {
  //loop control
  int count;

  //Output
  double sub_solar_epoch,
         surface_point[3],
         surface_vector[3];

  //Arrays that we return to Ruby
  VALUE rb_vector = rb_ary_new();
  VALUE rb_point  = rb_ary_new();

  subslr_c(StringValuePtr(method), StringValuePtr(target), NUM2DBL(et), StringValuePtr(fixref), StringValuePtr(abcorr), StringValuePtr(obsrvr), surface_point, &sub_solar_epoch, surface_vector);

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  for (count = 0; count < 3; count++) {
    rb_ary_push(rb_vector, DBL2NUM(surface_vector[count]));
    rb_ary_push(rb_point, DBL2NUM(surface_point[count]));
  }  

  return rb_ary_new3(3, rb_point, rb_vector, DBL2NUM(sub_solar_epoch));
}

/*

void getfov_c ( SpiceInt        instid,
                SpiceInt        room,
                SpiceInt        shapelen,
                SpiceInt        framelen,
                SpiceChar     * shape,
                SpiceChar     * frame,
                SpiceDouble     bsight [3],
                SpiceInt      * n,
                SpiceDouble     bounds [][3] )

*/

static VALUE getfov(VALUE self, VALUE instid, VALUE room, VALUE shapelen, VALUE framelen) {
  int count, vector_count;
  //Maximum size of SPICE ID is 32 chars
  char shape[32], frame[32];
  
  //Upper bound on boundary vectors is uncertain
  double boundary_sight[3];
  double boundary_vectors [10][3];
  
  VALUE rb_bounds; 
  VALUE rb_sight_vector = rb_ary_new();

  getfov_c(FIX2INT(instid), FIX2INT(room), FIX2INT(shapelen), FIX2INT(framelen), shape, frame, boundary_sight, &vector_count, boundary_vectors);

  for (count = 0; count < 3; count++) {
    rb_ary_push(rb_sight_vector, DBL2NUM(boundary_sight[count]));
  }

  rb_bounds = rb_ary_new2(vector_count);

  for (count = 0; count < vector_count; count++) {
    rb_ary_push(rb_bounds, rb_ary_new3(3, DBL2NUM(boundary_vectors[count][0]), DBL2NUM(boundary_vectors[count][1]), DBL2NUM(boundary_vectors[count][2])));
  }
  
  if(spice_error(SPICE_ERROR_SHORT)) {
    return Qnil;
  }

  return rb_ary_new3(4, RB_STR2SYM(shape), RB_STR2SYM(frame), rb_sight_vector, rb_bounds, vector_count);
}

static VALUE recsph(VALUE self, VALUE rectangular) {
  double radius, 
         colatitude, 
         longitude;  

  recsph_c(NM_STORAGE_DENSE(rectangular)->elements, &radius, &colatitude, &longitude);

  return rb_ary_new3(3, DBL2NUM(radius), DBL2NUM(colatitude), DBL2NUM(longitude));
}
//End of Geometry and Co-Ordinate Functions



//Time and Time Conversions Functions

static VALUE str2et(VALUE self, VALUE epoch) {
  double ephemeris_time;

  str2et_c(StringValuePtr(epoch), &ephemeris_time);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;
  
  return DBL2NUM(ephemeris_time);
}


void Init_spice_rub(){
  spicerub_module = rb_define_module("SpiceRub");
  
  //Attach Kernel Loading functions to module 
  rb_define_module_function(spicerub_module, "furnsh", furnsh, 1);
  rb_define_module_function(spicerub_module, "ktotal", ktotal, -1);
  rb_define_module_function(spicerub_module, "unload", unload, 1);
  rb_define_module_function(spicerub_module, "kclear", kclear, 0);

  //Attach Geometry-Coordinate functions to module
  rb_define_module_function(spicerub_module, "latrec", latrec, 4);
  rb_define_module_function(spicerub_module, "reclat", reclat, 1);
  rb_define_module_function(spicerub_module, "lspcn", lspcn, -1);
  rb_define_module_function(spicerub_module, "sincpt", sincpt, 8);
  rb_define_module_function(spicerub_module, "subpnt", subpnt, 6);
  rb_define_module_function(spicerub_module, "subslr", subslr, 6);
  rb_define_module_function(spicerub_module, "getfov", getfov, 4);
  rb_define_module_function(spicerub_module, "recsph", recsph, 1);

  
  //Atttach Time and Time Conversion functions
  rb_define_module_function(spicerub_module, "str2et", str2et, 1);
  

  rb_spice_error = rb_define_class("SpiceError", rb_eStandardError);
}
