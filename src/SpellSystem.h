// SpellSystem.h
// System for managing cooldowns and passive spells

#ifndef SPELLSYSTEM_H
#define SPELLSYSTEM_H

#include "Game.h"
#include "Scripting.h"

// Update all spell components each frame
class SpellSystem : public ECS::EntitySystem {
  public:

    // Register this system in the world
    static void registerSpellSystem(sol::environment& env, ECS::World* world) {

      // Create and install spell system
      env.set_function("useSpellSystem", [&env, world]() { 

        // Debug message
        Console::log("Initialising Spell System..");

        // Create the camera system to return to the world
        auto* newSPS = new SpellSystem();
        world->registerSystem(newSPS);
      });
    }

    // Update spell components
    virtual void update(ECS::World* world, const sf::Time& dt) override {

      // Get every entity with a sprite and transform
      world->each<Abilities> ( [&](ECS::Entity* e, ECS::ComponentHandle<Abilities> a) {
        Abilities& abilities = a.get();
        abilities.updateAllSpells(dt);
      });
    }
};


#endif
