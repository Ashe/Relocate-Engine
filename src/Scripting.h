// Scripting.h
// Functions to be called in a script

#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <algorithm>
#include <string>

#include "Sol.h"
#include "ECS.h"
#include "Game.h"

namespace Script {

  // Begin using Lua and register functions
  void startLua();

  // Register scene-specific functions
  void registerSceneFunctions(sol::environment& env, ECS::World* world);

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

    ////////////////////////////////
    // ENTITY COMPONENT FUNCTIONS //
    ////////////////////////////////

    // Generic component defaults for Lua
    template <typename T> T& assign(ECS::Entity* e) { return (e->assign<T>()).get(); }
    template <typename T> bool has(ECS::Entity* e) { return e->has<T>(); }
    template <typename T> T& get(ECS::Entity* e) { return (e->get<T>()).get(); }
    template <typename T> void remove(ECS::Entity* e) {e->remove<T>();}
  };

  // Convenience function for defining glue code in Lua
  // Convenience function for defining glue code in Lua
  template <typename T> void
  registerComponentToEntity(const std::string& name) {

    // Create simple functions for use in Lua
    Game::lua.new_usertype<ECS::Entity>("Entity",
      "assign" + name, &Funcs::assign<T>,
      "has" + name, &Funcs::has<T>,
      "get" + name, &Funcs::get<T>,
      "remove" + name, &Funcs::remove<T>
    );
  }
  template <typename T> void
  registerComponentToEntity(sol::environment& env, const std::string& name) {

    // Create simple functions for use in Lua
    env.new_usertype<ECS::Entity>("Entity",
      "assign" + name, &Funcs::assign<T>,
      "has" + name, &Funcs::has<T>,
      "get" + name, &Funcs::get<T>,
      "remove" + name, &Funcs::remove<T>
    );
  }
};

#endif
