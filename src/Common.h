// Common.h
// Miscellaneous classes and functions

#ifndef COMMON_H
#define COMMON_H

#include "ECS.h"
#include "Sol.h"

// Events for IMGUI
struct addDebugMenuEntryEvent {};
struct addDebugInfoEvent {};

// Base class for all components
class Component {
  public:
    Component(ECS::Entity* e)
      : owner_(e) {}
    Component(const Component& other) : owner_(other.owner_) {}
    void operator=(const Component& other) {}
  protected:
    ECS::Entity* const owner_;
};

// Base class for all drawable objects
struct Renderable : public sf::Drawable{
  int depth = 0;
  virtual void draw(sf::RenderTarget& t, sf::RenderStates s) const override = 0;
};

// Compare Renderable functor
struct cmpRenderable {
  bool operator()(const Renderable* left, const Renderable* right) const {
    return left->depth > right->depth;
  }
};

#endif
