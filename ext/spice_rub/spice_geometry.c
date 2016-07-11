#include "spice_geometry.h"

VALUE sr_latrec(VALUE self, VALUE radius, VALUE longitude, VALUE latitude) {
  double vector[3];
  
  latrec_c(NUM2DBL(radius), NUM2DBL(longitude), NUM2DBL(latitude),  vector);
  
  return rb_nmatrix_dense_create(FLOAT64, (size_t *) GEO_VECTOR_SHAPE, 2, (void *) vector, 3);
}

VALUE sr_reclat(VALUE self, VALUE rectangular_point) {
  double radius, 
         longitude, 
         latitude;

  reclat_c(NM_STORAGE_DENSE(rectangular_point)->elements, &radius, &longitude, &latitude);
  
  return rb_ary_new3(3, DBL2NUM(radius), DBL2NUM(longitude), DBL2NUM(latitude));
}


//TODO : Test this, current kernel files do not cover this 
VALUE sr_lspcn(int argc, VALUE *argv, VALUE self) {
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
VALUE sr_sincpt(VALUE self, VALUE method, VALUE target, VALUE et, VALUE fixref, VALUE abcorr, VALUE obsrvr, VALUE dref, VALUE dvec) {
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
  
  rb_point = rb_nmatrix_dense_create(FLOAT64, (size_t *) GEO_VECTOR_SHAPE, 2, (void *) surface_point, 3);
  rb_vector = rb_nmatrix_dense_create(FLOAT64, (size_t *) GEO_VECTOR_SHAPE, 2, (void *) surface_vector, 3);


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

VALUE sr_subpnt(VALUE self, VALUE method, VALUE target, VALUE et, VALUE fixref, VALUE abcorr, VALUE obsrvr) {
  //Output
  double observer_epoch,
         surface_point[3],
         surface_vector[3];

  //Arrays that we return to Ruby
  VALUE rb_vector;
  VALUE rb_point;

  subpnt_c(StringValuePtr(method), RB_SYM2STR(target), NUM2DBL(et), RB_SYM2STR(fixref), RB_SYM2STR(abcorr), RB_SYM2STR(obsrvr), surface_point, &observer_epoch, surface_vector);

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;
  
  rb_vector = rb_nmatrix_dense_create(FLOAT64, (size_t *) GEO_VECTOR_SHAPE, 2, (void *) surface_point, 3);
  rb_point = rb_nmatrix_dense_create(FLOAT64, (size_t *) GEO_VECTOR_SHAPE, 2, (void *) surface_vector, 3);
  
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

VALUE sr_subslr(VALUE self, VALUE method, VALUE target, VALUE et, VALUE fixref, VALUE abcorr, VALUE obsrvr) {
  //Output
  double sub_solar_epoch,
         surface_point[3],
         surface_vector[3];

  //Arrays that we return to Ruby
  VALUE rb_vector = rb_ary_new();
  VALUE rb_point  = rb_ary_new();

  subslr_c(StringValuePtr(method), RB_SYM2STR(target), NUM2DBL(et), RB_SYM2STR(fixref), RB_SYM2STR(abcorr), RB_SYM2STR(obsrvr), surface_point, &sub_solar_epoch, surface_vector);

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  rb_point = rb_nmatrix_dense_create(FLOAT64, (size_t *) GEO_VECTOR_SHAPE, 2, (void *) surface_point, 3);
  rb_vector = rb_nmatrix_dense_create(FLOAT64, (size_t *) GEO_VECTOR_SHAPE, 2, (void *) surface_vector, 3);

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

VALUE sr_getfov(VALUE self, VALUE instid, VALUE room, VALUE shapelen, VALUE framelen) {
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
  
  rb_sight_vector = rb_nmatrix_dense_create(FLOAT64, (size_t *) GEO_VECTOR_SHAPE, 2, (void *) boundary_sight, 3); 

  rb_bounds = rb_ary_new2(vector_count);

  for (count = 0; count < vector_count; count++) {
    rb_ary_push(rb_bounds, rb_nmatrix_dense_create(FLOAT64, (size_t *) GEO_VECTOR_SHAPE, 2, (void *) boundary_vectors[count], 3));
  }

  rb_shape = RB_STR2SYM(shape);
  rb_frame = RB_STR2SYM(frame);

  xfree(shape);
  xfree(frame);

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil; 
  
  return rb_ary_new3(5, rb_shape, rb_frame, rb_sight_vector, rb_bounds, INT2FIX(vector_count));
}

VALUE sr_recsph(VALUE self, VALUE rectangular) {
  double radius, 
         colatitude, 
         longitude;  

  recsph_c(NM_STORAGE_DENSE(rectangular)->elements, &radius, &colatitude, &longitude);

  return rb_ary_new3(3, DBL2NUM(radius), DBL2NUM(colatitude), DBL2NUM(longitude));
}

VALUE sr_sphrec(VALUE self, VALUE radius, VALUE colatitude, VALUE longitude) {
  double vector[3];

  sphrec_c(NUM2DBL(radius), NUM2DBL(colatitude), NUM2DBL(longitude), vector);
  
  return rb_nmatrix_dense_create(FLOAT64, (size_t *) GEO_VECTOR_SHAPE, 2, (void *) vector, 3);
}

/*
SpiceDouble phaseq_c ( SpiceDouble       et,
                       ConstSpiceChar  * target,
                       ConstSpiceChar  * illmn,
                       ConstSpiceChar  * obsrvr,
                       ConstSpiceChar  * abcorr )

*/
VALUE sr_phaseq(VALUE self, VALUE et, VALUE target, VALUE illmn, VALUE obsrvr, VALUE abcorr) {
  double phase_angle;

  phase_angle = phaseq_c(NUM2DBL(et), RB_SYM2STR(target), RB_SYM2STR(illmn), RB_SYM2STR(obsrvr), RB_SYM2STR(abcorr));

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  return DBL2NUM(phase_angle);
}

VALUE sr_recrad(VALUE self, VALUE rectangular) {
  double range,
         right_ascension,
         declination;

  recrad_c(NM_STORAGE_DENSE(rectangular)->elements, &range, &right_ascension, &declination);

  return rb_ary_new3(3, DBL2NUM(range), DBL2NUM(right_ascension), DBL2NUM(declination));
}

VALUE sr_radrec(VALUE self, VALUE range, VALUE right_ascension, VALUE declination) {
  double vector[3];

  radrec_c(NUM2DBL(range), NUM2DBL(right_ascension), NUM2DBL(declination), vector);
  
  return rb_nmatrix_dense_create(FLOAT64, (size_t *) GEO_VECTOR_SHAPE, 2, (void *) vector, 3);
}

VALUE sr_recgeo(VALUE self, VALUE rectangular, VALUE radius_equatorial, VALUE flattening) {
  double longitude,
         latitude,
         altitude;

  recgeo_c(NM_STORAGE_DENSE(rectangular)->elements, NUM2DBL(radius_equatorial), NUM2DBL(flattening), &longitude, &latitude, &altitude);       
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  return rb_ary_new3(3, DBL2NUM(longitude), DBL2NUM(latitude), DBL2NUM(altitude));
}

VALUE sr_georec(VALUE self, VALUE longitude, VALUE latitude, VALUE altitude, VALUE radius_equatorial, VALUE flattening) {
  double vector[3];

  georec_c(NUM2DBL(longitude), NUM2DBL(latitude), NUM2DBL(altitude), NUM2DBL(radius_equatorial), NUM2DBL(flattening), vector);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  return rb_nmatrix_dense_create(FLOAT64, (size_t *) GEO_VECTOR_SHAPE, 2, (void *) vector, 3);
}

VALUE sr_recpgr(VALUE self, VALUE body, VALUE rectangular, VALUE radius_equatorial, VALUE flattening) {
  double longitude,
         latitude,
         altitude;

  recpgr_c(RB_SYM2STR(body), NM_STORAGE_DENSE(rectangular)->elements, NUM2DBL(radius_equatorial), NUM2DBL(flattening), &longitude, &latitude, &altitude);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  return rb_ary_new3(3, DBL2NUM(longitude), DBL2NUM(latitude), DBL2NUM(altitude));
}

VALUE sr_pgrrec(VALUE self, VALUE body, VALUE longitude, VALUE latitude, VALUE altitude, VALUE radius_equatorial, VALUE flattening) {
  double vector[3];

  pgrrec_c(RB_SYM2STR(body), NUM2DBL(longitude), NUM2DBL(latitude), NUM2DBL(altitude), NUM2DBL(radius_equatorial), NUM2DBL(flattening),  vector);

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  return rb_nmatrix_dense_create(FLOAT64, (size_t *) GEO_VECTOR_SHAPE, 2, (void *) vector, 3);
}

VALUE sr_srfrec(VALUE self, VALUE body, VALUE longitude, VALUE latitude) {
  double vector[3];

  srfrec_c(FIX2INT(body), NUM2DBL(longitude), NUM2DBL(latitude), vector);

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  return rb_nmatrix_dense_create(FLOAT64, (size_t *) GEO_VECTOR_SHAPE, 2, (void *) vector, 3);
}

VALUE sr_dpr(VALUE self) {
  return DBL2NUM(dpr_c());
}

VALUE sr_rpd(VALUE self) {
  return DBL2NUM(rpd_c());
}

VALUE sr_bodvrd(VALUE self, VALUE bodynm, VALUE item, VALUE maxn) {
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

VALUE sr_bodvcd(VALUE self, VALUE bodynm, VALUE item, VALUE maxn) {
  int dim, count;
  double * values = ALLOC_N(double, maxn); 
  VALUE rb_values = rb_ary_new();

  bodvcd_c(FIX2INT(bodynm), RB_SYM2STR(item), FIX2INT(maxn), &dim, values);
  
  for(count = 0 ; count < dim ; count++) {
    rb_ary_push(rb_values, DBL2NUM(values[count]));
  }
  
  xfree(values);

  return rb_ary_new3(2, INT2FIX(dim), rb_values);
}

VALUE sr_latsph(VALUE self, VALUE radius, VALUE longitude, VALUE latitude) {
  double rho, 
         colat, 
         lons;

  latsph_c(NUM2DBL(radius), NUM2DBL(longitude), NUM2DBL(latitude),  &rho, &colat, &lons);
  
  return rb_ary_new3(3, DBL2NUM(rho), DBL2NUM(colat), DBL2NUM(lons));
}

VALUE sr_sphlat(VALUE self, VALUE radius, VALUE colatitude, VALUE longitude) {
  double lat_radius,
         lat_longitude,
         lat_latitude;

  sphlat_c(NUM2DBL(radius), NUM2DBL(colatitude), NUM2DBL(longitude), &lat_radius, &lat_longitude, &lat_latitude);

  return rb_ary_new3(3, DBL2NUM(lat_radius), DBL2NUM(lat_longitude), DBL2NUM(lat_latitude));
}
