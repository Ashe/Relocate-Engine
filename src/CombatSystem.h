// CombatSystem.h
// System that monitors health and combatants

#ifndef COMBATSYSTEM_H
#define COMBATSYSTEM_H

#include "Game.h"
#include "Scripting.h"

#include "Combat.h"
#include "Possession.h"
#include "Sprite.h"
#include "Expire.h"

// Monitor health's of entities
class CombatSystem : public ECS::EntitySystem {
  public:

    // Register this system in the world
    static void registerCombatSystem(sol::environment& env, ECS::World* world) {

      // Create and install combat system
      env.set_function("useCombatSystem", [&env, world]() { 

        // Debug message
        Console::log("Initialising Combat System..");

        // Create the camera system to return to the world
        auto* newCBS = new CombatSystem();
        world->registerSystem(newCBS);
      });
    }

    // Manipulate the window's view every frame
    virtual void update(ECS::World* world, const sf::Time& dt) override {

      // Get every entity with a combat component
      world->each<Combat>([&](ECS::Entity* e, ECS::ComponentHandle<Combat> c) {

        // If the entity has died
        if (c->getCurrentHealth() <= 0) {

          // Play death animation if possible
          if (e->has<Sprite>()) {

            // Get the sprite and play a death animation
            auto sprite = e->get<Sprite>();
            sprite->playAnimation("death");

            // If desired, kill the entity after the animation
            if (c->stats.deleteOnDeath &&
              c->stats.deleteAfterAnimation &&
              sprite->hasFinishedAnimation()) {

                Expire::expireEntity(e, c->stats.deathDelay);
            }

            // Lock the animation to this death animation
            sprite->lockAnimation = true;
            sprite->setLooped(false);
          }

          // If it has no sprite, delete now even if it 
          // requested to wait for the animation
          else if (c->stats.deleteOnDeath) {
            Expire::expireEntity(e, c->stats.deathDelay);
          }

          // Remove control of dead entities
          if (e->has<Possession>()) {
            e->remove<Possession>();
            Console::log("[Note] Possessed entity has died.");
          }
        }
      });
    }
};

#endif
