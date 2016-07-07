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

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  return rb_nmatrix_dense_create(FLOAT64, (size_t *) rotation_shape , 2, (void *) position_transform , 9);
}

VALUE sr_pxfrm2(VALUE self, VALUE from , VALUE to , VALUE epoch_at, VALUE epoch_to) {
  double position_transform[3][3];
  size_t rotation_shape[2] = {3,3};

  pxfrm2_c(RB_SYM2STR(from), RB_SYM2STR(to), NUM2DBL(epoch_at), NUM2DBL(epoch_to), position_transform);

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  return rb_nmatrix_dense_create(FLOAT64, (size_t *) rotation_shape , 2, (void *) position_transform , 9);
}

VALUE sr_sxform(VALUE self, VALUE from , VALUE to , VALUE at) {
  double state_transform[6][6];
  size_t rotation_shape[2] = {6,6};

  sxform_c(RB_SYM2STR(from), RB_SYM2STR(to), NUM2DBL(at), state_transform);

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  return rb_nmatrix_dense_create(FLOAT64, (size_t *) rotation_shape , 2, (void *) state_transform , 36);
}

VALUE sr_spkcpo(VALUE self, VALUE target, VALUE et, VALUE outref, VALUE refloc, VALUE abcorr, VALUE obspos, VALUE obsctr, VALUE obsref) {
  double state[6], light_time;
  VALUE rb_state;
  
  spkcpo_c( RB_SYM2STR(target), 
            NUM2DBL(et), 
            RB_SYM2STR(outref), 
            RB_SYM2STR(refloc),
            RB_SYM2STR(abcorr),
            NM_STORAGE_DENSE(obspos)->elements,
            RB_SYM2STR(obsctr),
            RB_SYM2STR(obsref),
            state,
            &light_time );

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  rb_state = rb_nmatrix_dense_create(FLOAT64, (size_t *) EPHEM_STATE_SHAPE, 2, (void *) state, 6);
  
  return rb_ary_new3(2, rb_state, DBL2NUM(light_time)); 
}

VALUE sr_spkcpt(VALUE self, VALUE trgpos, VALUE trgctr, VALUE trgref, VALUE et, VALUE outref, VALUE refloc, VALUE abcorr, VALUE obsrvr) {
  double state[6], light_time;
  VALUE rb_state;  
  
  spkcpt_c( NM_STORAGE_DENSE(trgpos)->elements,
            RB_SYM2STR(trgctr),
            RB_SYM2STR(trgref),
            NUM2DBL(et),
            RB_SYM2STR(outref),
            RB_SYM2STR(refloc),
            RB_SYM2STR(abcorr),
            RB_SYM2STR(obsrvr),
            state,
            &light_time );

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  rb_state = rb_nmatrix_dense_create(FLOAT64, (size_t *) EPHEM_STATE_SHAPE, 2, (void *) state, 6);
  
  return rb_ary_new3(2, rb_state, DBL2NUM(light_time)); 
}

VALUE sr_spkcvo(VALUE self, VALUE target, VALUE et, VALUE outref, VALUE refloc, VALUE abcorr, VALUE obssta, VALUE obsepc, VALUE obsctr, VALUE obsref) {
  double state[6], light_time;
  VALUE rb_state;

  spkcvo_c( RB_SYM2STR(target),
            NUM2DBL(et),
            RB_SYM2STR(outref),
            RB_SYM2STR(refloc),
            RB_SYM2STR(abcorr),
            NM_STORAGE_DENSE(obssta)->elements,
            NUM2DBL(obsepc),
            RB_SYM2STR(obsctr),
            RB_SYM2STR(obsref),
            state,
            &light_time );
 
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  rb_state = rb_nmatrix_dense_create(FLOAT64, (size_t *) EPHEM_STATE_SHAPE, 2, (void *) state, 6);
  
  return rb_ary_new3(2, rb_state, DBL2NUM(light_time));
}

VALUE sr_spkcvt(VALUE self, VALUE trgsta, VALUE trgepc, VALUE trgctr, VALUE trgref, VALUE et, VALUE outref, VALUE refloc, VALUE abcorr, VALUE obsrvr) {
  double state[6], light_time;
  VALUE rb_state;

  spkcvt_c( NM_STORAGE_DENSE(trgsta)->elements,
            DBL2NUM(trgepc),
            RB_SYM2STR(trgctr),
            RB_SYM2STR(trgref),
            DBL2NUM(et),
            RB_SYM2STR(outref),
            RB_SYM2STR(refloc),
            RB_SYM2STR(abcorr),
            RB_SYM2STR(obsrvr),
            state,
            &light_time );

  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  rb_state = rb_nmatrix_dense_create(FLOAT64, (size_t *) EPHEM_STATE_SHAPE, 2, (void *) state, 6);
  
  return rb_ary_new3(2, rb_state, DBL2NUM(light_time));
}

VALUE sr_pckfrm(VALUE self, VALUE pck_file) {
  SPICEINT_CELL(output, 1000);

  pckfrm_c(StringValuePtr(pck_file), &output);
  
  return Qnil;
  //#TODO : Extract data from the SPICECELL
}  

VALUE sr_spkobj(VALUE self, VALUE spk_file) {
  SPICEINT_CELL(output, 1000);

  spkobj_c(StringValuePtr(spk_file), &output);  
  
  return Qnil;
  //#TODO : Extract data from the SPICECELL
}