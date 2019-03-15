// Scripting.h
// Functions to be called in a script

#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <SFML/Graphics.hpp>
#include "Sol.h"
#include "ECS.h"

namespace Script {

  // Begin using Guile and register functions
  void startLua();

  // Unregister functions that belong to a scene
  void unregisterSceneFunctions();

  namespace Funcs {

    ///////////////////////
    // TRIVIAL FUNCTIONS //
    ///////////////////////
    
    void emptyFunction();
    void emptyUpdateFunction(const sf::Time& dt);

    ////////////////////
    // SFML FUNCTIONS //
    ////////////////////
    
    sf::Vector2i makeVector2i(int i, int j);
  };
};

#endif
