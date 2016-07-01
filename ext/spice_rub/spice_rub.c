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

void Init_spice_rub() {
  spicerub_top_module = rb_define_module("SpiceRub");
  spicerub_nested_module = rb_define_module_under(spicerub_top_module, "Native");
  
  //Attach Kernel Loading functions to module 
  rb_define_module_function(spicerub_nested_module, "furnsh", sr_furnsh, 1);
  rb_define_module_function(spicerub_nested_module, "ktotal", sr_ktotal, -1);
  rb_define_module_function(spicerub_nested_module, "unload", sr_unload, 1);
  rb_define_module_function(spicerub_nested_module, "kclear", sr_kclear, 0);

  //Attach Geometry-Coordinate functions to module
  rb_define_module_function(spicerub_nested_module, "latrec", sr_latrec, 3);
  rb_define_module_function(spicerub_nested_module, "reclat", sr_reclat, 1);
  rb_define_module_function(spicerub_nested_module, "lspcn", sr_lspcn, -1);
  rb_define_module_function(spicerub_nested_module, "sincpt", sr_sincpt, 8);
  rb_define_module_function(spicerub_nested_module, "subpnt", sr_subpnt, 6);
  rb_define_module_function(spicerub_nested_module, "subslr", sr_subslr, 6);
  rb_define_module_function(spicerub_nested_module, "getfov", sr_getfov, 4);
  rb_define_module_function(spicerub_nested_module, "recsph", sr_recsph, 1);
  rb_define_module_function(spicerub_nested_module, "sphrec", sr_sphrec, 3);
  rb_define_module_function(spicerub_nested_module, "phaseq", sr_phaseq, 5);
  rb_define_module_function(spicerub_nested_module, "recrad", sr_recrad, 1);
  rb_define_module_function(spicerub_nested_module, "radrec", sr_radrec, 3);
  rb_define_module_function(spicerub_nested_module, "recgeo", sr_recgeo, 3);
  rb_define_module_function(spicerub_nested_module, "georec", sr_georec, 5);
  rb_define_module_function(spicerub_nested_module, "recpgr", sr_recpgr, 4);
  rb_define_module_function(spicerub_nested_module, "pgrrec", sr_pgrrec, 6);
  rb_define_module_function(spicerub_nested_module, "dpr", sr_dpr, 0);
  rb_define_module_function(spicerub_nested_module, "rpd", sr_rpd, 0);
  rb_define_module_function(spicerub_nested_module, "bodvrd", sr_bodvrd, 3);
  rb_define_module_function(spicerub_nested_module, "bodvcd", sr_bodvcd, 3);


  //Atttach Time and Time Conversion functions
  rb_define_module_function(spicerub_nested_module, "str2et", sr_str2et, 1);
  rb_define_module_function(spicerub_nested_module, "gfdist", sr_gfdist, 9);
  rb_define_module_function(spicerub_nested_module, "gfsntc", sr_gfsntc, 15);
  rb_define_module_function(spicerub_nested_module, "gfsep", sr_gfsep, 14);
  rb_define_module_function(spicerub_nested_module, "gftfov", sr_gftfov, 8);
  rb_define_module_function(spicerub_nested_module, "gfoclt", sr_gfoclt, 11);
  rb_define_module_function(spicerub_nested_module, "gfrfov", sr_gfrfov, 7);
  rb_define_module_function(spicerub_nested_module, "timout", sr_timout, 3);
  rb_define_module_function(spicerub_nested_module, "sce2c", sr_sce2c, 2);
  rb_define_module_function(spicerub_nested_module, "sctiks", sr_sctiks, 2);
  rb_define_module_function(spicerub_nested_module, "scencd", sr_scencd, 2);
  rb_define_module_function(spicerub_nested_module, "scs2e", sr_scs2e, 2);
  rb_define_module_function(spicerub_nested_module, "scdecd", sr_scdecd, 3);
  rb_define_module_function(spicerub_nested_module, "sct2e", sr_sct2e , 2);
  rb_define_module_function(spicerub_nested_module, "spd", sr_spd, 0);

  //Attach Ephemerides routines
  rb_define_module_function(spicerub_nested_module, "spkpos", sr_spkpos , 5);
  rb_define_module_function(spicerub_nested_module, "spkezr", sr_spkezr , 5);
  rb_define_module_function(spicerub_nested_module, "spkcpt", sr_spkcpt , 8);
  rb_define_module_function(spicerub_nested_module, "spkcvo", sr_spkcvo , 9);
  rb_define_module_function(spicerub_nested_module, "spkcpo", sr_spkcpo , 8);
  rb_define_module_function(spicerub_nested_module, "pxform", sr_pxform , 3);
  rb_define_module_function(spicerub_nested_module, "sxform", sr_sxform , 3);
  
  rb_spice_error = rb_define_class("SpiceError", rb_eStandardError);
}
