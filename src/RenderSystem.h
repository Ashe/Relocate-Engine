// RenderSystem.h
// Places sprites wherever their transforms are

#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <math.h>

#include "Game.h"
#include "Scripting.h"

#include "Sprite.h"
#include "Text.h"
#include "Transform.h"

// Every frame, move sprites to their transform's locations
class RenderSystem : public ECS::EntitySystem {
  public:

    // Register this system in the world
    static void registerRenderSystem(sol::environment& env, ECS::World* world) {

      // Create and install sprite system
      env.set_function("useRenderSystem", [&env, world]() { 

        // Debug message
        Console::log("Initialising Render System..");

        // Create the camera system to return to the world
        auto* newRS = new RenderSystem();
        world->registerSystem(newRS);
      });
    }

    // Manipulate the sprite's transform every frame
    virtual void update(ECS::World* world, const sf::Time& dt) override {

      // Get every entity with a sprite and transform
      world->each<Sprite, Transform>( 
        [&](ECS::Entity* e, ECS::ComponentHandle<Sprite> s, ECS::ComponentHandle<Transform> t) {

        // Move sprite and then update the animation
        repositionTransformable(e, t.get(), (sf::Transformable*)&s.get());
        s->updateAnimation(dt);

      });

      // Get every entity with a sprite and transform
      world->each<Text, Transform>( 
        [&](ECS::Entity* e, ECS::ComponentHandle<Text> txt, ECS::ComponentHandle<Transform> t) {

        // Move text
        repositionTransformable(e, t.get(), (sf::Transformable*)&txt.get());

      });
    }

    // Convenience function for moving renderable objects
    static void repositionTransformable(ECS::Entity* e, Transform& t, sf::Transformable* c) {

      // If the renderable is part of the UI, the transform acts as an offset
      sf::Vector2f offset = sf::Vector2f();
      if (e->has<UIWidget>()) {
        const sf::Vector2f anchor = e->get<UIWidget>()->anchor;
        const sf::Vector2f center = Game::view.getCenter();
        const sf::Vector2f size = Game::view.getSize();
        offset.x = center.x + (anchor.x * size.x * 0.5f);
        offset.y = center.y + (anchor.y * size.y * 0.5f);
      }

      // Finally, move the renderable
      c->setPosition(t.position + offset);
      c->setRotation(t.rotation);
    }
};

#endif
