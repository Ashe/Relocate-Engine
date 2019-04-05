// Transform.h
// A component containing position and rotation of an entity

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Game.h"
#include "Scripting.h"

// The position of which things should be rendered at
class Transform {
  public:

    // Make this component scriptable
    static void registerFunctions() {

      // Register the usual assign, has, remove functions to Entity
      Script::registerComponentToEntity<Transform>("Transform");

      // Create the Transform usertype
      Game::lua.new_usertype<Transform>("Transform",
        "position", &Transform::position,
        "rotation", &Transform::rotation
      );
    }

    // Constructors
    Transform(float x = 0.f, float y = 0.f, float r = 0.f)
      : position(sf::Vector2f(x, y))
      , rotation(r) {}
    Transform(const sf::Vector2f& pos, float r = 0.f)
      : position(pos)
      , rotation(r) {}

    // Position of the entity to render
    sf::Vector2f position;

    // Rotation of the entity to render
    float rotation;
};

#endif
