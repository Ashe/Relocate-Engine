// Transform.h
// A component containing position and rotation of an entity

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Game.h"

class Transform {
  public:

    // Register assign method
    static Transform& assign(ECS::Entity* e) {
      return (e->assign<Transform>()).get();
    }

    // Make this component scriptable
    static void registerFunctions() {

      // Make a way of assigning a transform to an entity
      Game::lua.set_function("assignTransform", &assign);

      // Create the Transform type
      Game::lua.new_usertype<Transform>("Transform",
        sol::constructors<Transform(float, float, float)>(),
        "position", &Transform::position,
        "rotation", &Transform::rotation
      );
    }

    // Constructor
    Transform(float x = 0.f, float y = 0.f, float r = 0.f)
      : position(sf::Vector2f(x, y))
      , rotation(r) {}

    sf::Vector2i position;
    float rotation;
};

#endif
