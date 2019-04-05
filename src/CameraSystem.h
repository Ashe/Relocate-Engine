// CameraSystem.h
// System for manipulating view based on entities

#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include <functional>

#include "Game.h"
#include "Scripting.h"

#include "Camera.h"
#include "Transform.h"

// System which finds Cameras and sets view to that entity's location
class CameraSystem : public ECS::EntitySystem {
  public:

    // Register Camera System in the world
    static void registerCameraSystemFunctions(sol::environment& env, ECS::World* world);

    // Manipulate the window's view every frame
    virtual void update(ECS::World* world, const sf::Time& dt) override;
};


#endif
