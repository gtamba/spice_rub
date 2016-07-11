//Top level IMPLICIT Declarations

VALUE spicerub_top_module;
VALUE spicerub_nested_module;
VALUE rb_spice_error;

//Kernel Loading Functions
VALUE sr_furnsh(VALUE self, VALUE kernel);
VALUE sr_unload(VALUE self, VALUE kernel);
VALUE sr_ktotal(int argc, VALUE *argv, VALUE self);
VALUE sr_kclear(VALUE self);

//Geometry and Co-ordinate System Function
VALUE sr_latrec(VALUE self, VALUE radius, VALUE longtitude, VALUE latitude);
VALUE sr_lspcn(int argc, VALUE *argv, VALUE self);
VALUE sr_reclat(VALUE self, VALUE rectangular_point);
VALUE sr_sincpt(VALUE self, VALUE method, VALUE target, VALUE et, VALUE fixref, VALUE abcorr, VALUE obsrvr, VALUE dref, VALUE dvec);
VALUE sr_subpnt(VALUE self, VALUE method, VALUE target, VALUE et, VALUE fixref, VALUE abcorr, VALUE obsrvr);
VALUE sr_subslr(VALUE self, VALUE method, VALUE target, VALUE et, VALUE fixref, VALUE abcorr, VALUE obsrvr);
VALUE sr_recsph(VALUE self, VALUE rectangular);
VALUE sr_sphrec(VALUE self, VALUE radius, VALUE colatitude, VALUE longitude);
VALUE sr_sphlat(VALUE self, VALUE radius, VALUE colatitude, VALUE longitude);
VALUE sr_latsph(VALUE self, VALUE radius, VALUE longtitude, VALUE latitude);
VALUE sr_phaseq(VALUE self, VALUE et, VALUE target, VALUE illmn, VALUE obsrvr, VALUE abcorr);
VALUE sr_recrad(VALUE self, VALUE rectangular);
VALUE sr_radrec(VALUE self, VALUE range, VALUE right_ascension, VALUE declination);
VALUE sr_recgeo(VALUE self, VALUE rectangular, VALUE radius_equatorial, VALUE flattening);
VALUE sr_georec(VALUE self, VALUE longitude, VALUE latitude, VALUE altitude, VALUE radius_equatorial, VALUE flattening);
VALUE sr_recpgr(VALUE self, VALUE body, VALUE rectangular, VALUE radius_equatorial, VALUE flattening);
VALUE sr_pgrrec(VALUE self, VALUE body, VALUE longitude, VALUE latitude, VALUE altitude, VALUE radius_equatorial, VALUE flattening);
VALUE sr_dpr(VALUE self);
VALUE sr_rpd(VALUE self);
VALUE sr_getfov(VALUE self, VALUE instid, VALUE room, VALUE shapelen, VALUE framelen);
VALUE sr_bodvrd(VALUE self, VALUE bodynm, VALUE item, VALUE maxn);
VALUE sr_bodvcd(VALUE self, VALUE bodynm, VALUE item, VALUE maxn);


//Time and Time Conversions Functions
VALUE sr_str2et(VALUE self, VALUE epoch);
VALUE sr_gfdist(VALUE self, VALUE target, VALUE abcorr, VALUE obsrvr, VALUE relate, VALUE refval, VALUE adjust, VALUE step, VALUE nintvls, VALUE confines);
VALUE sr_gfsntc(VALUE self, VALUE target, VALUE fixref, VALUE method, VALUE abcorr, VALUE obsrvr, VALUE dref, VALUE dvec, VALUE crdsys, VALUE coord, VALUE relate, VALUE refval, VALUE adjust, VALUE step, VALUE nintvls, VALUE confines);
VALUE sr_gfsep(VALUE self, VALUE target1, VALUE shape1, VALUE frame1, VALUE target2, VALUE shape2, VALUE frame2, VALUE abcorr, VALUE obsrvr, VALUE relate, VALUE refval, VALUE adjust, VALUE step, VALUE nintvls, VALUE confines);
VALUE sr_gftfov(VALUE self, VALUE inst, VALUE target, VALUE tshape, VALUE tframe, VALUE abcorr, VALUE obsrvr, VALUE step, VALUE confines);
VALUE sr_gfrfov(VALUE self, VALUE inst, VALUE raydir, VALUE rframe, VALUE abcorr, VALUE obsrvr, VALUE step, VALUE confines);
VALUE sr_timout(VALUE self, VALUE et, VALUE pictur, VALUE lenout);
VALUE sr_sce2c (VALUE self, VALUE sc, VALUE epoch);
VALUE sr_sctiks(VALUE self, VALUE sc, VALUE clkstr);
VALUE sr_scencd(VALUE self, VALUE sc, VALUE sclkch);
VALUE sr_scs2e(VALUE self, VALUE sc, VALUE sclkch);
VALUE sr_scdecd(VALUE self, VALUE sc, VALUE sclkdp, VALUE lenout);
VALUE sr_sct2e(VALUE self, VALUE sc, VALUE sclkdp);
VALUE sr_gfoclt(VALUE self, VALUE occtyp, VALUE front, VALUE fshape, VALUE fframe, VALUE back, VALUE bshape, VALUE bframe, VALUE abcorr, VALUE obsrvr, VALUE step, VALUE confines);
VALUE sr_spd(VALUE self);

//Ephemerides Function Declarations
VALUE sr_spkpos(VALUE self, VALUE targ, VALUE et, VALUE ref, VALUE abcorr, VALUE obs);
VALUE sr_spkezr(VALUE self, VALUE targ, VALUE et, VALUE ref, VALUE abcorr, VALUE obs);
VALUE sr_spkcpo(VALUE self, VALUE target, VALUE et, VALUE outref, VALUE refloc, VALUE abcorr, VALUE obspos, VALUE obsctr, VALUE obsref);
VALUE sr_spkcvo(VALUE self, VALUE target, VALUE et, VALUE outref, VALUE refloc, VALUE abcorr, VALUE obssta, VALUE obsepc, VALUE obsctr, VALUE obsref);
VALUE sr_spkcpt(VALUE self, VALUE trgpos, VALUE trgctr, VALUE trgref, VALUE et, VALUE outref, VALUE refloc, VALUE abcorr, VALUE obsrvr);
VALUE sr_spkcvt(VALUE self, VALUE trgsta, VALUE trgepc, VALUE trgctr, VALUE trgref, VALUE et, VALUE outref, VALUE refloc, VALUE abcorr, VALUE obsrvr);
VALUE sr_pxform(VALUE self, VALUE from , VALUE to , VALUE at);
VALUE sr_sxform(VALUE self, VALUE from , VALUE to , VALUE at);
VALUE sr_pxfrm2(VALUE self, VALUE from , VALUE to , VALUE epoch_at, VALUE epoch_to);
VALUE sr_spkobj(VALUE self, VALUE spk_file);
VALUE sr_pckfrm(VALUE self, VALUE pck_file);