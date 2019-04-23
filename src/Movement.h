// Movement.h
// A component that defines how an entity can move in the game

#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "Game.h"
#include "Scripting.h"

// Easy container for related stats
struct MovementStats {
  float movementSpeed = 0.f;
  float sprintSpeedMult = 2.f;
  float flightSpeed = 0.f;
  bool canSprint = false;
  bool canJump = false;
  bool canFly = false;
  bool canSprintWhileFlying = false;

  void showDebugInformation() {
    ImGui::Text("Movement speed: %f", movementSpeed);
    ImGui::Text("Sprint multiplier: %f", sprintSpeedMult);
    ImGui::Text("Flight speed: %f", flightSpeed);
    ImGui::Text("Can Sprint: %s", canSprint ? "true": "false");
    ImGui::Text("Can Jump: %s", canJump ? "true": "false");
    ImGui::Text("Can Fly: %s", canFly ? "true": "false");
    ImGui::Text("Can Sprint in air: %s", canSprintWhileFlying ? "true": "false");
  }
};

// Component to allow movement
class Movement : Component {
  public:

    // Make this component scriptable
    static void registerMovementType(sol::environment& env) {

      // Register the usual assign, has and remove functions
      Script::registerComponentToEntity<Movement>(env, "Movement");

      // Create the Movement user type
      env.new_usertype<MovementStats>("MovementStats",
        sol::constructors<MovementStats()>(),
        "movementSpeed", &MovementStats::movementSpeed,
        "canJump", &MovementStats::canJump,
        "sprintSpeedMult", &MovementStats::sprintSpeedMult,
        "canSprint", &MovementStats::canSprint,
        "flightSpeed", &MovementStats::flightSpeed,
        "canFly", &MovementStats::canFly,
        "canSprintWhileFlying", &MovementStats::canSprintWhileFlying
      );

      // Create the Movement user type
      env.new_usertype<Movement>("Movement",
        "stats", &Movement::stats,
        "isSprinting", &Movement::isSprinting
      );
    }

    // Constructor
    Movement(ECS::Entity* e) 
      : Component(e)
      , stats(MovementStats())
      , isSprinting(false) {
    }

    // Movement stats
    MovementStats stats;
    bool isSprinting;

    // Shows the debug information to ImGui
    void showDebugInformation() {
      ImGui::NextColumn();
      ImGui::Text("Is Sprinting: %s", isSprinting ? "true": "false");
      stats.showDebugInformation();
      ImGui::PushItemWidth(-1);
      ImGui::PopItemWidth();
      ImGui::NextColumn();
    }
};

#endif
