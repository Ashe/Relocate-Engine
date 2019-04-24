// CombatSystem.h
// System that monitors health and combatants

#ifndef COMBATSYSTEM_H
#define COMBATSYSTEM_H

#include "Game.h"
#include "Scripting.h"

#include "Combat.h"
#include "Possession.h"
#include "Sprite.h"

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

          // Delete immediately if desired
          if (c->stats.deleteImmediatelyOnDeath) {
            world->destroy(e);
          }
          else {

            // The entity isn't deleted instantaneously, so remove control of it
            if (e->has<Possession>()) {
              e->remove<Possession>();
              Console::log("[Note] Possessed entity has died.");
            }

            // If the sprite has a sprite to animate a death
            if (e->has<Sprite>()) {

              // Get the sprite and play a death animation
              auto sprite = e->get<Sprite>();

              // If we want to delete the animation after death we need a callback
              if (c->stats.deleteAfterDeathAnimation) {
                std::function<void()> death = [world, e, sprite]() { sprite->resetCallback(); world->destroy(e); };
                sprite->playAnimationWithCallback("death", death);
              }
              else {
                sprite->playAnimation("death");
              }

              // Lock the animation to this death animation
              sprite->lockAnimation = true;
              sprite->setLooped(false);
            }
          }
        }
      });
    }
};

#endif
