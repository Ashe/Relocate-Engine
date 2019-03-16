// Scripting.h
// Functions to be called in a script

#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <SFML/Graphics.hpp>
#include "Sol.h"
#include "ECS.h"

namespace Script {

  // Begin using Lua and register functions
  void startLua();

  // Register scene-specific functions
  void registerSceneFunctions(ECS::World* world);

  // Unregister functions that belong to a scene
  void unregisterSceneFunctions();

  namespace Funcs {

    ///////////////////////
    // TRIVIAL FUNCTIONS //
    ///////////////////////
    
    void emptyFunction();
    void emptyUpdateFunction(const sf::Time& dt);
    void emptyWindowEventFunction(const sf::Event& ev);

    ////////////////////
    // SFML FUNCTIONS //
    ////////////////////
    
    void registerVectors();
    void registerEvents();
  };
};

#endif
