#include "ruby.h"
#include "SpiceUsr.h"
#include "signal.h"
#include <stdbool.h>

//Macros for switch parameters in error message reports
#define SPICE_ERROR_SHORT 0
#define SPICE_ERROR_LONG 1
#define SPICE_ERROR_EXPLAIN 2
#define SPICE_ERROR_ALL 3

//Top level IMPLICIT Declarations

VALUE spicerub_module;
VALUE rb_spice_error;

static VALUE furnsh(int argc, VALUE *argv, VALUE self);
static VALUE unload(int argc, VALUE *argv, VALUE self);
static VALUE ktotal(int argc, VALUE *argv, VALUE self);
static VALUE kclear(VALUE self);

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

static VALUE furnsh(int argc, VALUE *argv, VALUE self) {
  sigset_t old_mask = block_signals();

  furnsh_c(StringValuePtr(argv[0]));

  restore_signals(old_mask);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qfalse;

  return Qtrue;
}

static VALUE unload(int argc, VALUE *argv, VALUE self) {
  sigset_t old_mask = block_signals();

  unload_c(StringValuePtr(argv[0]));
  
  restore_signals(old_mask);
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qfalse;

  return Qtrue;
}

static VALUE ktotal(int argc, VALUE *argv, VALUE self) {
  SpiceInt kernel_count;
  
  if(argc == 0) ktotal_c("ALL", &kernel_count);

  //Else convert Symbol to ID, ID to string if category argument supplied
  else ktotal_c(rb_id2name(SYM2ID(argv[0])), &kernel_count);

  spice_error(SPICE_ERROR_SHORT);

  return INT2FIX(kernel_count);
}

static VALUE kclear(VALUE self) {
  
  kclear_c();
  
  if(spice_error(SPICE_ERROR_SHORT)) return Qfalse;

  return Qtrue;
}

void Init_spice_rub(){
  spicerub_module = rb_define_module("SpiceRub");
  rb_define_module_function(spicerub_module, "furnsh", furnsh, -1);
  rb_define_module_function(spicerub_module, "ktotal", ktotal, -1);
  rb_define_module_function(spicerub_module, "unload", unload, -1);
  rb_define_module_function(spicerub_module, "kclear", kclear, -1);

  rb_spice_error = rb_define_class("SpiceError", rb_eStandardError);
}
