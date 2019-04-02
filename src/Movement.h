// Movement.h
// A component that defines how an entity can move in the game

#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "Game.h"
#include "Scripting.h"

// Component to allow movement
class Movement {
  public:

    // Make this component scriptable
    static void registerFunctions() {

      // Register the usual assign, has and remove functions
      Script::registerComponentToEntity<Movement>("Movement");

      // Create the Movement user type
      Game::lua.new_usertype<Movement>("Movement",
        "movementSpeed", &Movement::movementSpeed,
        "canJump", &Movement::canJump,
        "sprintSpeedMult", &Movement::sprintSpeedMult,
        "canSprint", &Movement::canSprint,
        "isSprinting", &Movement::isSprinting,
        "flightSpeed", &Movement::flightSpeed,
        "canFly", &Movement::canFly,
        "canSprintWhileFlying", &Movement::canSprintWhileFlying
      );
    }

    // Constructor
    Movement() 
    : movementSpeed(0.f)
    , canJump(false)
    , sprintSpeedMult(1.f)
    , canSprint(false)
    , isSprinting(false)
    , flightSpeed(0.f)
    , canFly(0.f) 
    , canSprintWhileFlying(0.f){}

    // Movement speed
    float movementSpeed;

    // Jump mechanics
    bool canJump;

    // Whether the entity can sprint on the ground
    float sprintSpeedMult;
    bool canSprint;
    bool isSprinting;

    // Whether the entity can fly
    float flightSpeed;
    bool canFly;
    bool canSprintWhileFlying;
};

#endif
