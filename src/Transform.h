// Transform.h
// A component containing position and rotation of an entity

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Game.h"
#include "Scripting.h"

class Transform {
  public:

    // Make this component scriptable
    static void registerFunctions() {

      // Make a way of assigning a transform to an entity
      Script::registerComponentDefaults<Transform>("Transform");

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

    sf::Vector2f position;
    float rotation;
};

#endif
