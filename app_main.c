/* -*- mode: c;  -*-
   file: app_main.c
*/

#include <stdlib.h>
#include <math.h>
#include "app_main.h"

void run_swank();

/* TODO: Are embedded quotes really needed? */
char start_swank[] =
  "\"/home/rett/dev-local/xoamorph/xara-lx/start-swank-server.lisp\"";

char* argv;
char** pargv;

int app_main() {
  argv = "app";
  pargv = &argv;
  cl_boot(1, pargv);
  atexit(cl_shutdown);
  /* Set up handler for Lisp errors to prevent buggy Lisp (an */
  /* imposibility, I know!) from killing the app. */
  const cl_env_ptr l_env = ecl_process_env();
  CL_CATCH_ALL_BEGIN(l_env) {
    CL_UNWIND_PROTECT_BEGIN(l_env) {
      run_swank();
    }
    CL_UNWIND_PROTECT_EXIT {}
    CL_UNWIND_PROTECT_END;
  }
  CL_CATCH_ALL_END;
  return 0;
}

void run_swank() {
  cl_object cl_start_swank_path = c_string_to_object(start_swank);
  cl_object cl_load =  ecl_make_symbol("LOAD","CL");
  cl_funcall(2, cl_load, cl_start_swank_path);
  return;
}
