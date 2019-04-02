// ControlSystem.h
// System to control possessed entities in the scene

#ifndef CONTROLSYSTEM_H
#define CONTROLSYSTEM_H

#include "Game.h"
#include "Scripting.h"

#include "Possession.h"
#include "RigidBody.h"
#include "Movement.h"

// System that manipulates possessed entities
class ControlSystem : public ECS::EntitySystem {
  public:

    // Register Control System in the world
    static void registerControlSystemFunctions(sol::environment& env, ECS::World* world);

    // Handle input and store meaningful actions
    static void handleInput(const sf::Event& ev);

    // Constructors
    ControlSystem();
    ~ControlSystem();

    // All control systems will 'know' about input
    static sf::Vector2f inputAxis;
    static bool isSprinting;
    static bool isJumping;

    // Manipulate all possessed components using their data
    virtual void update(ECS::World* world, const sf::Time& dt) override;
};

#endif
