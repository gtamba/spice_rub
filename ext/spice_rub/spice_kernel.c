#include "spice_kernel.h"

VALUE sr_furnsh(VALUE self, VALUE kernel) {
  sigset_t old_mask = block_signals();

  furnsh_c(StringValuePtr(kernel));

  restore_signals(old_mask);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qfalse;

  return Qtrue;
}

VALUE sr_unload(VALUE self, VALUE kernel) {
  sigset_t old_mask = block_signals();

  unload_c(StringValuePtr(kernel));
  
  restore_signals(old_mask);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qfalse;

  return Qtrue;
}

VALUE sr_ktotal(int argc, VALUE *argv, VALUE self) {
  SpiceInt kernel_count;
  
  if(argc == 0) ktotal_c("ALL", &kernel_count);

  //Else convert Symbol to ID, ID to string if category argument supplied
  else ktotal_c(RB_SYM2STR(argv[0]), &kernel_count);

  spice_error(SPICE_ERROR_SHORT);

  return INT2FIX(kernel_count);
}

VALUE sr_kclear(VALUE self) {
  
  kclear_c();
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qfalse;

  return Qtrue;
}