// Scripting.h
// Functions to be called in a script

#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <libguile.h>
#include <SFML/Graphics.hpp>
#include "ECS.h"

namespace Script {

  // Begin using Guile and register functions
  void startGuile();

  // Test function for guile
  SCM testGuile();
}

#endif
