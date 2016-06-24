#include "ruby.h"
#include "SpiceUsr.h"
#include "signal.h"
#include <stdbool.h>
#include "spice_rub.h"
#include "spice_rub_utils.h"
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

static VALUE latrec(VALUE self, VALUE radius, VALUE longitude, VALUE latitude) {
  double vector[3];
  
  latrec_c(NUM2DBL(radius), NUM2DBL(longitude), NUM2DBL(latitude),  vector);
  
  return rb_nmatrix_dense_create(FLOAT64, (size_t *) VECTOR_SHAPE, 2, (void *) vector, 3);
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

  result = lspcn_c(RB_SYM2STR(argv[0]), NUM2DBL(argv[1]), RB_SYM2STR(argv[2]));

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
  //Output parameters
  SpiceBoolean found;
  double intercept_epoch,
         surface_point[3],
         surface_vector[3];

  //Arrays that we return to Ruby
  VALUE rb_vector;
  VALUE rb_point;

  sincpt_c(StringValuePtr(method), StringValuePtr(target), NUM2DBL(et), StringValuePtr(fixref), StringValuePtr(abcorr), StringValuePtr(obsrvr), StringValuePtr(dref), NM_STORAGE_DENSE(dvec)->elements, surface_point, &intercept_epoch, surface_vector, &found);

  if(!found) {
  	return Qfalse;
  }

  else if(spice_error(SPICE_ERROR_SHORT)) {
  	return Qnil;
  }
  
  rb_point = rb_nmatrix_dense_create(FLOAT64, (size_t *) VECTOR_SHAPE, 2, (void *) surface_point, 3);
  rb_vector = rb_nmatrix_dense_create(FLOAT64, (size_t *) VECTOR_SHAPE, 2, (void *) surface_vector, 3);


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
  //Output
  double observer_epoch,
         surface_point[3],
         surface_vector[3];

  //Arrays that we return to Ruby
  VALUE rb_vector;
  VALUE rb_point;

  subpnt_c(StringValuePtr(method), RB_SYM2STR(target), NUM2DBL(et), RB_SYM2STR(fixref), RB_SYM2STR(abcorr), RB_SYM2STR(obsrvr), surface_point, &observer_epoch, surface_vector);

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;
  
  rb_vector = rb_nmatrix_dense_create(FLOAT64, (size_t *) VECTOR_SHAPE, 2, (void *) surface_point, 3);
  rb_point = rb_nmatrix_dense_create(FLOAT64, (size_t *) VECTOR_SHAPE, 2, (void *) surface_vector, 3);
  
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
  //Output
  double sub_solar_epoch,
         surface_point[3],
         surface_vector[3];

  //Arrays that we return to Ruby
  VALUE rb_vector = rb_ary_new();
  VALUE rb_point  = rb_ary_new();

  subslr_c(StringValuePtr(method), RB_SYM2STR(target), NUM2DBL(et), RB_SYM2STR(fixref), RB_SYM2STR(abcorr), RB_SYM2STR(obsrvr), surface_point, &sub_solar_epoch, surface_vector);

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  rb_point = rb_nmatrix_dense_create(FLOAT64, (size_t *) VECTOR_SHAPE, 2, (void *) surface_point, 3);
  rb_vector = rb_nmatrix_dense_create(FLOAT64, (size_t *) VECTOR_SHAPE, 2, (void *) surface_vector, 3);

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
  char * shape = ALLOC_N(char, shapelen);
  char * frame = ALLOC_N(char, framelen);
  
  //Upper bound on boundary vectors is uncertain
  double boundary_sight[3];
  double boundary_vectors [10][3];
  
  VALUE rb_bounds; 
  VALUE rb_sight_vector = rb_ary_new();
  VALUE rb_shape, rb_frame;

  getfov_c(FIX2INT(instid), FIX2INT(room), FIX2INT(shapelen), FIX2INT(framelen), shape, frame, boundary_sight, &vector_count, boundary_vectors);
  
  if(spice_error(SPICE_ERROR_SHORT)) {
    return Qnil;
  }
  
  rb_sight_vector = rb_nmatrix_dense_create(FLOAT64, (size_t *) VECTOR_SHAPE, 2, (void *) boundary_sight, 3); 

  rb_bounds = rb_ary_new2(vector_count);

  for (count = 0; count < vector_count; count++) {
    rb_ary_push(rb_bounds, rb_nmatrix_dense_create(FLOAT64, (size_t *) VECTOR_SHAPE, 2, (void *) boundary_vectors[count], 3));
  }

  rb_shape = RB_STR2SYM(shape);
  rb_frame = RB_STR2SYM(frame);

  xfree(shape);
  xfree(frame);

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil; 
  
  return rb_ary_new3(5, rb_shape, rb_frame, rb_sight_vector, rb_bounds, INT2FIX(vector_count));
}

static VALUE recsph(VALUE self, VALUE rectangular) {
  double radius, 
         colatitude, 
         longitude;  

  recsph_c(NM_STORAGE_DENSE(rectangular)->elements, &radius, &colatitude, &longitude);

  return rb_ary_new3(3, DBL2NUM(radius), DBL2NUM(colatitude), DBL2NUM(longitude));
}

static VALUE sphrec(VALUE self, VALUE radius, VALUE colatitude, VALUE longitude) {
  double vector[3];

  sphrec_c(NUM2DBL(radius), NUM2DBL(colatitude), NUM2DBL(longitude), vector);
  
  return rb_nmatrix_dense_create(FLOAT64, (size_t *) VECTOR_SHAPE, 2, (void *) vector, 3);
}

/*
SpiceDouble phaseq_c ( SpiceDouble       et,
                       ConstSpiceChar  * target,
                       ConstSpiceChar  * illmn,
                       ConstSpiceChar  * obsrvr,
                       ConstSpiceChar  * abcorr )

*/
static VALUE phaseq(VALUE self, VALUE et, VALUE target, VALUE illmn, VALUE obsrvr, VALUE abcorr) {
  double phase_angle;

  phase_angle = phaseq_c(NUM2DBL(et), RB_SYM2STR(target), RB_SYM2STR(illmn), RB_SYM2STR(obsrvr), RB_SYM2STR(abcorr));

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  return DBL2NUM(phase_angle);
}

static VALUE recrad(VALUE self, VALUE rectangular) {
  double range,
         right_ascension,
         declination;

  recrad_c(NM_STORAGE_DENSE(rectangular)->elements, &range, &right_ascension, &declination);

  return rb_ary_new3(3, DBL2NUM(range), DBL2NUM(right_ascension), DBL2NUM(declination));
}

static VALUE radrec(VALUE self, VALUE range, VALUE right_ascension, VALUE declination) {
  double vector[3];

  radrec_c(NUM2DBL(range), NUM2DBL(right_ascension), NUM2DBL(declination), vector);
  
  return rb_nmatrix_dense_create(FLOAT64, (size_t *) VECTOR_SHAPE, 2, (void *) vector, 3);
}

static VALUE recgeo(VALUE self, VALUE rectangular, VALUE radius_equatorial, VALUE flattening) {
  double longitude,
         latitude,
         altitude;

  recgeo_c(NM_STORAGE_DENSE(rectangular)->elements, NUM2DBL(radius_equatorial), NUM2DBL(flattening), &longitude, &latitude, &altitude);       
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  return rb_ary_new3(3, DBL2NUM(longitude), DBL2NUM(latitude), DBL2NUM(altitude));
}

static VALUE georec(VALUE self, VALUE longitude, VALUE latitude, VALUE altitude, VALUE radius_equatorial, VALUE flattening) {
  double vector[3];

  georec_c(NUM2DBL(longitude), NUM2DBL(latitude), NUM2DBL(altitude), NUM2DBL(radius_equatorial), NUM2DBL(flattening), vector);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  return rb_nmatrix_dense_create(FLOAT64, (size_t *) VECTOR_SHAPE, 2, (void *) vector, 3);
}

static VALUE recpgr(VALUE self, VALUE body, VALUE rectangular, VALUE radius_equatorial, VALUE flattening) {
  double longitude,
         latitude,
         altitude;

  recpgr_c(RB_SYM2STR(body), NM_STORAGE_DENSE(rectangular)->elements, NUM2DBL(radius_equatorial), NUM2DBL(flattening), &longitude, &latitude, &altitude);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  return rb_ary_new3(3, DBL2NUM(longitude), DBL2NUM(latitude), DBL2NUM(altitude));
}

static VALUE pgrrec(VALUE self, VALUE body, VALUE longitude, VALUE latitude, VALUE altitude, VALUE radius_equatorial, VALUE flattening) {
  double vector[3];

  pgrrec_c(RB_SYM2STR(body), NUM2DBL(radius_equatorial), NUM2DBL(flattening), NUM2DBL(longitude), NUM2DBL(latitude), NUM2DBL(altitude), vector);

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  return rb_nmatrix_dense_create(FLOAT64, (size_t *) VECTOR_SHAPE, 2, (void *) vector, 3);
}

static VALUE dpr(VALUE self) {
  return DBL2NUM(dpr_c());
}

static VALUE rpd(VALUE self) {
  return DBL2NUM(rpd_c());
}

static VALUE bodvrd(VALUE self, VALUE bodynm, VALUE item, VALUE maxn) {
  int dim, count;
  double * values = ALLOC_N(double, maxn); 
  VALUE rb_values = rb_ary_new();

  bodvrd_c(RB_SYM2STR(bodynm), RB_SYM2STR(item), FIX2INT(maxn), &dim, values);
  
  for(count = 0 ; count < dim ; count++) {
    rb_ary_push(rb_values, DBL2NUM(values[count]));
  }
  
  xfree(values);

  return rb_ary_new3(2, INT2FIX(dim), rb_values);
}

//End of Geometry and Co-Ordinate Functions



//Time and Time Conversions Functions

static VALUE gfdist(VALUE self, VALUE target, VALUE abcorr, VALUE obsrvr, VALUE relate, VALUE refval, VALUE adjust, VALUE step, VALUE nintvls, VALUE confines) {
  int count, interval_count;
  double beginning, end, et0, et1;
  VALUE result;
  
  SPICEDOUBLE_CELL(intervals, 5000);
  SPICEDOUBLE_CELL(window, 5000);
  
  et0 = NUM2DBL(RARRAY_AREF(confines, 0));
  et1 = NUM2DBL(RARRAY_AREF(confines,1));

  wninsd_c(et0, et1, &window);
  
  gfdist_c( RB_SYM2STR(target), 
            RB_SYM2STR(abcorr), 
            RB_SYM2STR(obsrvr), 
            RB_SYM2STR(relate), 
            NUM2DBL(refval), 
            NUM2DBL(adjust), 
            NUM2DBL(step), 
            FIX2INT(nintvls),
            &window , 
            &intervals );
  
  if (spice_error(SPICE_ERROR_SHORT)) return Qnil;

  interval_count = wncard_c(&intervals);
  
  if (interval_count == 0) return Qnil;

  result = rb_ary_new2(interval_count);

  for (count = 0; count < interval_count; count++) {
    wnfetd_c(&intervals, count, &beginning, &end);
    rb_ary_push(result, rb_ary_new3(2, DBL2NUM(beginning), DBL2NUM(end)));
  }

  return result;
}

static VALUE gfsntc(VALUE self, VALUE target, VALUE fixref, VALUE method, VALUE abcorr, VALUE obsrvr, VALUE dref, 
                    VALUE dvec, VALUE crdsys, VALUE coord, VALUE relate, VALUE refval, VALUE adjust, VALUE step, VALUE nintvls, VALUE confines) {
  
  int count, interval_count;
  double beginning, end, et0, et1;
  VALUE result;
  
  SPICEDOUBLE_CELL(intervals, 5000);
  SPICEDOUBLE_CELL(window, 5000);
  
  et0 = NUM2DBL(RARRAY_AREF(confines, 0));
  et1 = NUM2DBL(RARRAY_AREF(confines,1));

  wninsd_c(et0, et1, &window);
  
  gfsntc_c( RB_SYM2STR(target), 
            RB_SYM2STR(fixref),
            RB_SYM2STR(method),
            RB_SYM2STR(abcorr), 
            RB_SYM2STR(obsrvr), 
            RB_SYM2STR(dref),
            NM_STORAGE_DENSE(dvec)->elements,
            RB_SYM2STR(crdsys), 
            RB_SYM2STR(coord),
            RB_SYM2STR(relate), 
            NUM2DBL(refval), 
            NUM2DBL(adjust), 
            NUM2DBL(step), 
            FIX2INT(nintvls),
            &window , 
            &intervals );
  
  if (spice_error(SPICE_ERROR_SHORT)) return Qnil;

  interval_count = wncard_c(&intervals);
  
  if (interval_count == 0) return Qnil;

  result = rb_ary_new2(interval_count);

  for (count = 0; count < interval_count; count++) {
    wnfetd_c(&intervals, count, &beginning, &end);
    rb_ary_push(result, rb_ary_new3(2, DBL2NUM(beginning), DBL2NUM(end)));
  }

  return result;
}

static VALUE gfsep(VALUE self, VALUE target1, VALUE shape1, VALUE frame1, VALUE target2, VALUE shape2, VALUE frame2, 
                   VALUE abcorr, VALUE obsrvr, VALUE relate, VALUE refval, VALUE adjust, VALUE step, VALUE nintvls, VALUE confines) {
  
  int count, interval_count;
  double beginning, end, et0, et1;
  VALUE result;
  
  SPICEDOUBLE_CELL(intervals, 5000);
  SPICEDOUBLE_CELL(window, 5000);
  
  et0 = NUM2DBL(RARRAY_AREF(confines, 0));
  et1 = NUM2DBL(RARRAY_AREF(confines,1));

  wninsd_c(et0, et1, &window);
  
  gfsep_c( RB_SYM2STR(target1),
            RB_SYM2STR(shape1),
            RB_SYM2STR(frame1),
            RB_SYM2STR(target2),
            RB_SYM2STR(shape2),
            RB_SYM2STR(frame2), 
            RB_SYM2STR(abcorr), 
            RB_SYM2STR(obsrvr), 
            RB_SYM2STR(relate), 
            NUM2DBL(refval), 
            NUM2DBL(adjust), 
            NUM2DBL(step), 
            FIX2INT(nintvls),
            &window , 
            &intervals );
  
  if (spice_error(SPICE_ERROR_SHORT)) return Qnil;

  interval_count = wncard_c(&intervals);
  
  if (interval_count == 0) return Qnil;

  result = rb_ary_new2(interval_count);

  for (count = 0; count < interval_count; count++) {
    wnfetd_c(&intervals, count, &beginning, &end);
    rb_ary_push(result, rb_ary_new3(2, DBL2NUM(beginning), DBL2NUM(end)));
  }

  return result;
}

static VALUE gftfov(VALUE self, VALUE inst, VALUE target, VALUE tshape, VALUE tframe, VALUE abcorr, VALUE obsrvr, VALUE step, VALUE confines) {
  int count, interval_count;
  double beginning, end, et0, et1;
  VALUE result;
  
  SPICEDOUBLE_CELL(intervals, 5000);
  SPICEDOUBLE_CELL(window, 5000);
  
  et0 = NUM2DBL(RARRAY_AREF(confines, 0));
  et1 = NUM2DBL(RARRAY_AREF(confines,1));

  wninsd_c(et0, et1, &window);
  
  gftfov_c( RB_SYM2STR(inst), 
            RB_SYM2STR(target), 
            RB_SYM2STR(tshape), 
            RB_SYM2STR(tframe),
            RB_SYM2STR(abcorr),
            RB_SYM2STR(obsrvr), 
            NUM2DBL(step), 
            &window , 
            &intervals );
  
  if (spice_error(SPICE_ERROR_SHORT)) return Qnil;

  interval_count = wncard_c(&intervals);
  
  if (interval_count == 0) return Qnil;

  result = rb_ary_new2(interval_count);

  for (count = 0; count < interval_count; count++) {
    wnfetd_c(&intervals, count, &beginning, &end);
    rb_ary_push(result, rb_ary_new3(2, DBL2NUM(beginning), DBL2NUM(end)));
  }

  return result;
}

static VALUE gfrfov(VALUE self, VALUE inst, VALUE raydir, VALUE rframe, VALUE abcorr, VALUE obsrvr, VALUE step, VALUE confines) {
  int count, interval_count;
  double beginning, end, et0, et1;
  VALUE result;
  
  SPICEDOUBLE_CELL(intervals, 5000);
  SPICEDOUBLE_CELL(window, 5000);
  
  et0 = NUM2DBL(RARRAY_AREF(confines, 0));
  et1 = NUM2DBL(RARRAY_AREF(confines,1));

  wninsd_c(et0, et1, &window);
  
  gfrfov_c( RB_SYM2STR(inst), 
            NM_STORAGE_DENSE(raydir)->elements, 
            RB_SYM2STR(rframe), 
            RB_SYM2STR(abcorr),
            RB_SYM2STR(obsrvr), 
            NUM2DBL(step), 
            &window , 
            &intervals );
  
  if (spice_error(SPICE_ERROR_SHORT)) return Qnil;

  interval_count = wncard_c(&intervals);
  
  if (interval_count == 0) return Qnil;

  result = rb_ary_new2(interval_count);

  for (count = 0; count < interval_count; count++) {
    wnfetd_c(&intervals, count, &beginning, &end);
    rb_ary_push(result, rb_ary_new3(2, DBL2NUM(beginning), DBL2NUM(end)));
  }

  return result;
}

static VALUE gfoclt(VALUE self, VALUE occtyp, VALUE front, VALUE fshape, VALUE fframe, VALUE back, VALUE bshape, VALUE bframe, 
                    VALUE abcorr, VALUE obsrvr, VALUE step, VALUE confines) {
  
  int count, interval_count;
  double beginning, end, et0, et1;
  VALUE result;
  
  SPICEDOUBLE_CELL(intervals, 5000);
  SPICEDOUBLE_CELL(window, 5000);
  
  et0 = NUM2DBL(RARRAY_AREF(confines, 0));
  et1 = NUM2DBL(RARRAY_AREF(confines,1));

  wninsd_c(et0, et1, &window);
  
  gfoclt_c( RB_SYM2STR(occtyp),
            RB_SYM2STR(front), 
            RB_SYM2STR(fshape),
            RB_SYM2STR(fframe),
            RB_SYM2STR(back),
            RB_SYM2STR(bshape), 
            RB_SYM2STR(bframe),
            RB_SYM2STR(abcorr),
            RB_SYM2STR(obsrvr), 
            NUM2DBL(step), 
            &window , 
            &intervals );
  
  if (spice_error(SPICE_ERROR_SHORT)) return Qnil;

  interval_count = wncard_c(&intervals);
  
  if (interval_count == 0) return Qnil;

  result = rb_ary_new2(interval_count);

  for (count = 0; count < interval_count; count++) {
    wnfetd_c(&intervals, count, &beginning, &end);
    rb_ary_push(result, rb_ary_new3(2, DBL2NUM(beginning), DBL2NUM(end)));
  }

  return result;
}

static VALUE spd(VALUE self) {
  return DBL2NUM(spd_c());
}

static VALUE timout(VALUE self, VALUE et, VALUE pictur, VALUE lenout) {
  VALUE result;
  char * output = ALLOC_N(char, FIX2INT(lenout));

  timout_c(NUM2DBL(et), StringValuePtr(pictur), FIX2INT(lenout), output);
  result = rb_str_new2(output); 
  xfree(output);

  return result;
}

static VALUE str2et(VALUE self, VALUE epoch) {
  double ephemeris_time;

  str2et_c(StringValuePtr(epoch), &ephemeris_time);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;
  
  return DBL2NUM(ephemeris_time);
}

static VALUE sce2c(VALUE self, VALUE sc, VALUE epoch) {
  double result;

  sce2c_c(FIX2INT(sc), NUM2DBL(epoch), &result);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;
  
  return DBL2NUM(result);
}

static VALUE sctiks(VALUE self, VALUE sc, VALUE clkstr) {
  double result;

  sctiks_c(FIX2INT(sc), StringValuePtr(clkstr), &result);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;
  
  return DBL2NUM(result);
}

static VALUE scencd(VALUE self, VALUE sc, VALUE sclkch) {
  double result;

  scencd_c(FIX2INT(sc), StringValuePtr(sclkch), &result);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;
  
  return DBL2NUM(result);
}

static VALUE scs2e(VALUE self, VALUE sc, VALUE sclkch) {
  double result;

  scs2e_c(FIX2INT(sc), StringValuePtr(sclkch), &result);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;
  
  return DBL2NUM(result);
}

static VALUE scdecd(VALUE self, VALUE sc, VALUE sclkdp, VALUE lenout) {
  char * output = ALLOC_N(char, FIX2INT(lenout));
  VALUE result;

  scdecd_c(FIX2INT(sc), NUM2DBL(sclkdp), FIX2INT(lenout), output);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  result = rb_str_new2(output); 
  xfree(output);
 
  return result;
}

static VALUE sct2e(VALUE self, VALUE sc, VALUE sclkdp) {
  double output;

  sct2e_c(FIX2INT(sc), NUM2DBL(sclkdp), &output);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  return DBL2NUM(output);
}




void Init_spice_rub(){
  spicerub_top_module = rb_define_module("SpiceRub");
  spicerub_nested_module = rb_define_module_under(spicerub_top_module, "Native");
  
  //Attach Kernel Loading functions to module 
  rb_define_module_function(spicerub_nested_module, "furnsh", furnsh, 1);
  rb_define_module_function(spicerub_nested_module, "ktotal", ktotal, -1);
  rb_define_module_function(spicerub_nested_module, "unload", unload, 1);
  rb_define_module_function(spicerub_nested_module, "kclear", kclear, 0);

  //Attach Geometry-Coordinate functions to module
  rb_define_module_function(spicerub_nested_module, "latrec", latrec, 3);
  rb_define_module_function(spicerub_nested_module, "reclat", reclat, 1);
  rb_define_module_function(spicerub_nested_module, "lspcn", lspcn, -1);
  rb_define_module_function(spicerub_nested_module, "sincpt", sincpt, 8);
  rb_define_module_function(spicerub_nested_module, "subpnt", subpnt, 6);
  rb_define_module_function(spicerub_nested_module, "subslr", subslr, 6);
  rb_define_module_function(spicerub_nested_module, "getfov", getfov, 4);
  rb_define_module_function(spicerub_nested_module, "recsph", recsph, 1);
  rb_define_module_function(spicerub_nested_module, "sphrec", sphrec, 3);
  rb_define_module_function(spicerub_nested_module, "phaseq", phaseq, 5);
  rb_define_module_function(spicerub_nested_module, "recrad", recrad, 1);
  rb_define_module_function(spicerub_nested_module, "radrec", radrec, 3);
  rb_define_module_function(spicerub_nested_module, "recgeo", recgeo, 3);
  rb_define_module_function(spicerub_nested_module, "georec", georec, 5);
  rb_define_module_function(spicerub_nested_module, "recpgr", recpgr, 4);
  rb_define_module_function(spicerub_nested_module, "pgrrec", pgrrec, 6);
  rb_define_module_function(spicerub_nested_module, "dpr", dpr, 0);
  rb_define_module_function(spicerub_nested_module, "rpd", rpd, 0);
  rb_define_module_function(spicerub_nested_module, "bodvrd", bodvrd, 3);



  //Atttach Time and Time Conversion functions
  rb_define_module_function(spicerub_nested_module, "str2et", str2et, 1);
  rb_define_module_function(spicerub_nested_module, "gfdist", gfdist, 9);
  rb_define_module_function(spicerub_nested_module, "gfsntc", gfsntc, 15);
  rb_define_module_function(spicerub_nested_module, "gfsep", gfsep, 14);
  rb_define_module_function(spicerub_nested_module, "gftfov", gftfov, 8);
  rb_define_module_function(spicerub_nested_module, "gfoclt", gfoclt, 11);
  rb_define_module_function(spicerub_nested_module, "gfrfov", gfrfov, 7);
  rb_define_module_function(spicerub_nested_module, "timout", timout, 3);
  rb_define_module_function(spicerub_nested_module, "sce2c", sce2c, 2);
  rb_define_module_function(spicerub_nested_module, "sctiks", sctiks, 2);
  rb_define_module_function(spicerub_nested_module, "scencd", scencd, 2);
  rb_define_module_function(spicerub_nested_module, "scs2e", scs2e, 2);
  rb_define_module_function(spicerub_nested_module, "scdecd", scdecd, 3);
  rb_define_module_function(spicerub_nested_module, "sct2e", sct2e , 2);



  rb_define_module_function(spicerub_nested_module, "spd", spd, 0);

  rb_spice_error = rb_define_class("SpiceError", rb_eStandardError);
}
