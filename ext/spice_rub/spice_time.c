#include "spice_time.h"

VALUE sr_gfdist(VALUE self, VALUE target, VALUE abcorr, VALUE obsrvr, VALUE relate, VALUE refval, VALUE adjust, VALUE step, VALUE nintvls, VALUE confines) {
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

VALUE sr_gfsntc(VALUE self, VALUE target, VALUE fixref, VALUE method, VALUE abcorr, VALUE obsrvr, VALUE dref, 
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

VALUE sr_gfsep(VALUE self, VALUE target1, VALUE shape1, VALUE frame1, VALUE target2, VALUE shape2, VALUE frame2, 
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

VALUE sr_gftfov(VALUE self, VALUE inst, VALUE target, VALUE tshape, VALUE tframe, VALUE abcorr, VALUE obsrvr, VALUE step, VALUE confines) {
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

VALUE sr_gfrfov(VALUE self, VALUE inst, VALUE raydir, VALUE rframe, VALUE abcorr, VALUE obsrvr, VALUE step, VALUE confines) {
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

VALUE sr_gfoclt(VALUE self, VALUE occtyp, VALUE front, VALUE fshape, VALUE fframe, VALUE back, VALUE bshape, VALUE bframe, 
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

VALUE sr_spd(VALUE self) {
  return DBL2NUM(spd_c());
}

VALUE sr_timout(VALUE self, VALUE et, VALUE pictur, VALUE lenout) {
  VALUE result;
  char * output = ALLOC_N(char, FIX2INT(lenout));

  timout_c(NUM2DBL(et), StringValuePtr(pictur), FIX2INT(lenout), output);
  result = rb_str_new2(output); 
  xfree(output);

  return result;
}

VALUE sr_str2et(VALUE self, VALUE epoch) {
  double ephemeris_time;

  str2et_c(StringValuePtr(epoch), &ephemeris_time);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;
  
  return DBL2NUM(ephemeris_time);
}

VALUE sr_sce2c(VALUE self, VALUE sc, VALUE epoch) {
  double result;

  sce2c_c(FIX2INT(sc), NUM2DBL(epoch), &result);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;
  
  return DBL2NUM(result);
}

VALUE sr_sctiks(VALUE self, VALUE sc, VALUE clkstr) {
  double result;

  sctiks_c(FIX2INT(sc), StringValuePtr(clkstr), &result);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;
  
  return DBL2NUM(result);
}

VALUE sr_scencd(VALUE self, VALUE sc, VALUE sclkch) {
  double result;

  scencd_c(FIX2INT(sc), StringValuePtr(sclkch), &result);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;
  
  return DBL2NUM(result);
}

VALUE sr_scs2e(VALUE self, VALUE sc, VALUE sclkch) {
  double result;

  scs2e_c(FIX2INT(sc), StringValuePtr(sclkch), &result);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;
  
  return DBL2NUM(result);
}

VALUE sr_scdecd(VALUE self, VALUE sc, VALUE sclkdp, VALUE lenout) {
  char * output = ALLOC_N(char, FIX2INT(lenout));
  VALUE result;

  scdecd_c(FIX2INT(sc), NUM2DBL(sclkdp), FIX2INT(lenout), output);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  result = rb_str_new2(output); 
  xfree(output);
 
  return result;
}

VALUE sr_sct2e(VALUE self, VALUE sc, VALUE sclkdp) {
  double output;

  sct2e_c(FIX2INT(sc), NUM2DBL(sclkdp), &output);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qnil;

  return DBL2NUM(output);
}