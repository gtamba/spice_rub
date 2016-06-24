//Top level IMPLICIT Declarations

VALUE spicerub_top_module;
VALUE spicerub_nested_module;
VALUE rb_spice_error;

//Kernel Loading Functions
static VALUE furnsh(VALUE self, VALUE kernel);
static VALUE unload(VALUE self, VALUE kernel);
static VALUE ktotal(int argc, VALUE *argv, VALUE self);
static VALUE kclear(VALUE self);

//Geometry and Co-ordinate System Function
static VALUE latrec(VALUE self, VALUE radius, VALUE longtitude, VALUE latitude);
static VALUE  lspcn(int argc, VALUE *argv, VALUE self);
static VALUE reclat(VALUE self, VALUE rectangular_point);
static VALUE sincpt(VALUE self, VALUE method, VALUE target, VALUE et, VALUE fixref, VALUE abcorr, VALUE obsrvr, VALUE dref, VALUE dvec);
static VALUE subpnt(VALUE self, VALUE method, VALUE target, VALUE et, VALUE fixref, VALUE abcorr, VALUE obsrvr);
static VALUE subslr(VALUE self, VALUE method, VALUE target, VALUE et, VALUE fixref, VALUE abcorr, VALUE obsrvr);
static VALUE recsph(VALUE self, VALUE rectangular);
static VALUE sphrec(VALUE self, VALUE radius, VALUE colatitude, VALUE longitude);
static VALUE phaseq(VALUE self, VALUE et, VALUE target, VALUE illmn, VALUE obsrvr, VALUE abcorr);
static VALUE recrad(VALUE self, VALUE rectangular);
static VALUE radrec(VALUE self, VALUE range, VALUE right_ascension, VALUE declination);
static VALUE recgeo(VALUE self, VALUE rectangular, VALUE radius_equatorial, VALUE flattening);
static VALUE georec(VALUE self, VALUE longitude, VALUE latitude, VALUE altitude, VALUE radius_equatorial, VALUE flattening);
static VALUE recpgr(VALUE self, VALUE body, VALUE rectangular, VALUE radius_equatorial, VALUE flattening);
static VALUE pgrrec(VALUE self, VALUE body, VALUE longitude, VALUE latitude, VALUE altitude, VALUE radius_equatorial, VALUE flattening);
static VALUE dpr(VALUE self);
static VALUE rpd(VALUE self);
static VALUE getfov(VALUE self, VALUE instid, VALUE room, VALUE shapelen, VALUE framelen);
static VALUE bodvrd(VALUE self, VALUE bodynm, VALUE item, VALUE maxn);

//Time and Time Conversions Functions
static VALUE str2et(VALUE self, VALUE epoch);
static VALUE gfdist(VALUE self, VALUE target, VALUE abcorr, VALUE obsrvr, VALUE relate, VALUE refval, VALUE adjust, VALUE step, VALUE nintvls, VALUE confines);
static VALUE gfsntc(VALUE self, VALUE target, VALUE fixref, VALUE method, VALUE abcorr, VALUE obsrvr, VALUE dref, VALUE dvec, VALUE crdsys, VALUE coord, VALUE relate, VALUE refval, VALUE adjust, VALUE step, VALUE nintvls, VALUE confines);
static VALUE gfsep(VALUE self, VALUE target1, VALUE shape1, VALUE frame1, VALUE target2, VALUE shape2, VALUE frame2, VALUE abcorr, VALUE obsrvr, VALUE relate, VALUE refval, VALUE adjust, VALUE step, VALUE nintvls, VALUE confines);
static VALUE gftfov(VALUE self, VALUE inst, VALUE target, VALUE tshape, VALUE tframe, VALUE abcorr, VALUE obsrvr, VALUE step, VALUE confines);
static VALUE gfrfov(VALUE self, VALUE inst, VALUE raydir, VALUE rframe, VALUE abcorr, VALUE obsrvr, VALUE step, VALUE confines);

static VALUE spd(VALUE self);

//Helper Macros
