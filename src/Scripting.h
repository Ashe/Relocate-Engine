// Scripting.h
// Functions to be called in a script

#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <algorithm>
#include <string>

#include "Sol.h"
#include "ECS.h"
#include "Game.h"

// Everything to do with scripting goes in this namespace
namespace Script {

  // Begin using Lua and register functions
  void startLua();

  // Register scene-specific functions
  void registerSceneFunctions(sol::environment& env, ECS::World* world);

  // Helper functions are contained in this namespace
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
    template <typename T> T& assign(ECS::Entity* e) { return (e->assign<T>(e)).get(); }
    template <typename T> bool has(ECS::Entity* e) { return e->has<T>(); }
    template <typename T> T& get(ECS::Entity* e) { return (e->get<T>()).get(); }
    template <typename T> void remove(ECS::Entity* e) {e->remove<T>();}
  };

  // Convenience function for defining glue code in Lua
  template <typename T> void
  registerComponentToEntity(sol::environment& env, const std::string& name) {
    sol::usertype<ECS::Entity> entityType = Game::lua["Entity"];
    entityType.set("assign" + name, &Funcs::assign<T>);
    entityType.set("has" + name, &Funcs::has<T>);
    entityType.set("get" + name, &Funcs::get<T>);
    entityType.set("remove" + name, &Funcs::remove<T>);
  }
};

#endif
