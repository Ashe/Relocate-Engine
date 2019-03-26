// ControlSystem.cpp
// System to control possessed entities in the scene

#include "ControlSystem.h"

// Initialise static variables
sf::Vector2f ControlSystem::inputAxis = sf::Vector2f();

// Register Control System in the world
void
ControlSystem::registerControlSystemFunctions(sol::environment& env, ECS::World* world) {

  // Create and install physics system
  env.set_function("useControlSystem", [&env, world]() { 

    // Debug message
    if (Game::getDebugMode()) {
      printf("Initialising Control system..\n");
    }

    // Create the physics system to return to the world
    auto* newCS = new ControlSystem();
    world->registerSystem(newCS);
  });
}

// Constructors
ControlSystem::ControlSystem() {}
ControlSystem::~ControlSystem() {}

// Control all possessed entities
void
ControlSystem::update(ECS::World* world, const sf::Time& dt) {

  // Control the scene
  world->each<Possession>([&](ECS::Entity* e, ECS::ComponentHandle<Possession> p) {

    // If we have a RigidBody, we can move it
    auto r = e->get<RigidBody>();
    if (r.isValid()) {
      float XAxisSpeed = 0.005f;
      float YAxisSpeed = 0.015f;
      r->applyImpulseToCentre(inputAxis.x * XAxisSpeed, inputAxis.y * YAxisSpeed);
    }
  });
}
