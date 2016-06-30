#include "spice_ephemerides.h"

VALUE sr_spkpos(VALUE self, VALUE targ, VALUE et, VALUE ref, VALUE abcorr, VALUE obs) {
  double position[3], light_time;
  VALUE rb_position;

  spkpos_c(RB_SYM2STR(targ), NUM2DBL(et), RB_SYM2STR(ref), RB_SYM2STR(abcorr), RB_SYM2STR(obs), position, &light_time);

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  rb_position = rb_nmatrix_dense_create(FLOAT64, (size_t *) EPHEM_POSITION_SHAPE, 2, (void *) position, 3);

  return rb_ary_new3(2, rb_position, DBL2NUM(light_time));
}

VALUE sr_spkezr(VALUE self, VALUE targ, VALUE et, VALUE ref, VALUE abcorr, VALUE obs) {
  double state[6], light_time;
  VALUE rb_state;

  spkpos_c(RB_SYM2STR(targ), NUM2DBL(et), RB_SYM2STR(ref), RB_SYM2STR(abcorr), RB_SYM2STR(obs), state, &light_time);
 
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  rb_state = rb_nmatrix_dense_create(FLOAT64, (size_t *) EPHEM_STATE_SHAPE, 2, (void *) state, 6);

  return rb_ary_new3(2, rb_state, DBL2NUM(light_time));  
}

VALUE sr_pxform(VALUE self, VALUE from , VALUE to , VALUE at) {
  double position_transform[3][3];
  size_t rotation_shape[2] = {3,3};

  pxform_c(RB_SYM2STR(from), RB_SYM2STR(to), NUM2DBL(at), position_transform);

  return rb_nmatrix_dense_create(FLOAT64, (size_t *) rotation_shape , 2, (void *) position_transform , 9);
}

VALUE sr_sxform(VALUE self, VALUE from , VALUE to , VALUE at) {
  double state_transform[6][6];
  size_t rotation_shape[2] = {6,6};

  sxform_c(RB_SYM2STR(from), RB_SYM2STR(to), NUM2DBL(at), state_transform);

  return rb_nmatrix_dense_create(FLOAT64, (size_t *) rotation_shape , 2, (void *) state_transform , 36);
}