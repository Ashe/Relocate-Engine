// CameraSystem.cpp
// System for manipulating view based on entities

#include "CameraSystem.h"

// Register Control System in the world
void
CameraSystem::registerCameraSystemFunctions(sol::environment& env, ECS::World* world) {

  // Create and install camera system
  env.set_function("useCameraSystem", [&env, world]() { 

    // Debug message
    if (Game::getDebugMode()) {
      printf("Initialising Camera system..\n");
    }

    // Create the camera system to return to the world
    auto* newCS = new CameraSystem();
    world->registerSystem(newCS);
  });
}

// Follow entities with Cameras
void
CameraSystem::update(ECS::World* world, const sf::Time& dt) {

  // Find entities with cameras
  world->each<Camera, Transform>(
    [&](ECS::Entity* e, ECS::ComponentHandle<Camera> c, ECS::ComponentHandle<Transform> t) {

      // Combine position of transform and offset of camera
      Game::view.setCenter(t->position + c->offset);

  });
}
