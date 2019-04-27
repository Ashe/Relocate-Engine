// ExpirySystem.h
// Simple system for removing expired entities from the world

#ifndef EXPIRYSYSTEM_H
#define EXPIRYSYSTEM_H

#include "Game.h"
#include "Scripting.h"

#include "Expire.h"

// Update every expiry component every frame
class ExpirySystem : public ECS::EntitySystem {
  public:

    // Register this system in the world
    static void registerExpirySystem(sol::environment& env, ECS::World* world) {

      // Create and install sprite system
      env.set_function("useExpirySystem", [&env, world]() { 

        // Debug message
        Console::log("Initialising Expiry System..");

        // Create the camera system to return to the world
        auto* newEXS = new ExpirySystem();
        world->registerSystem(newEXS);
      });
    }

    // Delete any entities that have an expired Expire Component
    virtual void update(ECS::World* world, const sf::Time& dt) override {
      world->each<Expire>([&](ECS::Entity* e, ECS::ComponentHandle<Expire> x) {
        const bool remove = x->update(dt);
        if (remove) world->destroy(e);
      });
    }
};


#endif
