// Scripting.h
// Functions to be called in a script

#ifndef SCRIPTING_H
#define SCRIPTING_H

#include "Sol.h"
#include "ECS.h"
#include "Game.h"

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

    /////////////////////////
    // COMPONENT FUNCTIONS //
    /////////////////////////

    // Generic component defaults for Lua
    template <typename T> T& assign(ECS::Entity* e) { return (e->assign<T>()).get(); }
    template <typename T> T& get(ECS::Entity* e) { return (e->get<T>()).get(); }
    template <typename T> bool has(ECS::Entity* e) { return e->has<T>(); }
  };

  // Convenience function for defining glue code in Lua
  template <typename T> void
  registerComponentDefaults(const std::string& name) {
    Game::lua.set_function("assign" + name, &Funcs::assign<T>);
    Game::lua.set_function("get" + name, &Funcs::get<T>);
    Game::lua.set_function("has" + name, &Funcs::has<T>);
  }

};

#endif
