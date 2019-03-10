// Transform.h
// A component containing position and rotation of an entity

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <SFML/Graphics.hpp>

class Transform {
  public:
    Transform(float x = 0.f, float y = 0.f, float r = 0.f)
      : position(sf::Vector2f(x, y))
      , rotation(r) {}

    sf::Vector2i position;
    float rotation;
};

#endif
