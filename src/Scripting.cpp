// Scripting.cpp
// Functions to be called in a script

#include "Scripting.h"

// Avoid cyclic dependancies
#include "Game.h"

// Register functions to Guile
void
Script::startGuile() {
  printf("Initialising Guile..\n");
  scm_init_guile();
  scm_c_define_gsubr ("testGuile", 0, 0, 0, reinterpret_cast<void*>(&Script::testGuile));
}

// Simply test Guile
SCM
Script::testGuile() {
  printf("Guile successfully initialised.\n");
  return SCM_BOOL_T;
}
