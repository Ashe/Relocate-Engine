// SpriteSystem.h
// Places sprites wherever their transforms are

#ifndef SPRITESYSTEM_H
#define SPRITESYSTEM_H

#include <math.h>

#include "Game.h"
#include "Scripting.h"

#include "Sprite.h"
#include "Transform.h"

// Every frame, move sprites to their transform's locations
class SpriteSystem : public ECS::EntitySystem {
  public:

    // Register this system in the world
    static void registerSpriteSystem(sol::environment& env, ECS::World* world) {

      // Create and install sprite system
      env.set_function("useSpriteSystem", [&env, world]() { 

        // Debug message
        Console::log("Initialising Sprite System..");

        // Create the camera system to return to the world
        auto* newSPS = new SpriteSystem();
        world->registerSystem(newSPS);
      });
    }

    // Manipulate the sprite's transform every frame
    virtual void update(ECS::World* world, const sf::Time& dt) override {

      // Get every entity with a sprite and transform
      world->each<Sprite, Transform>( 
        [&](ECS::Entity* e, ECS::ComponentHandle<Sprite> s, ECS::ComponentHandle<Transform> t) {
          Sprite& sprite = s.get();
          sprite.setPosition(t->position);
          sprite.setRotation(t->rotation);
          sprite.updateAnimation(dt);
      });
    }
};

#endif
