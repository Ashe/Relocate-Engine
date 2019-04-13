// SpritePlacementSystem.h
// Places sprites wherever their transforms are

#ifndef SPRITEPLACEMENTSYSTEM_H
#define SPRITEPLACEMENTSYSTEM_H

#include <math.h>

#include "Game.h"
#include "Scripting.h"

#include "Sprite.h"
#include "Transform.h"

// Every frame, move sprites to their transform's locations
class SpritePlacementSystem : public ECS::EntitySystem {
  public:

    // Register this system in the world
    static void registerSpritePlacementSystem(sol::environment& env, ECS::World* world) {

      // Create and install camera system
      env.set_function("useSpritePlacementSystem", [&env, world]() { 

        // Debug message
        if (Game::getDebugMode()) {
          Console::log("Initialising Sprite Placement System system..");
        }

        // Create the camera system to return to the world
        auto* newSPS = new SpritePlacementSystem();
        world->registerSystem(newSPS);
      });
    }

    // Manipulate the window's view every frame
    virtual void update(ECS::World* world, const sf::Time& dt) override {

      // Get every entity with a sprite and transform
      world->each<Sprite, Transform>( 
        [&](ECS::Entity* e, ECS::ComponentHandle<Sprite> s, ECS::ComponentHandle<Transform> t) {
          Sprite& sprite = s.get();
          sprite.setPosition(t->position);
          sprite.setRotation(t->rotation);
      });
    }
};

#endif
