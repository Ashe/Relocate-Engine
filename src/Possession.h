// Possession.h
// A component signalling whether this entity is possessed by the player

#ifndef POSSESSION_H
#define POSSESSION_H

#include "Game.h"
#include "Scripting.h"

// Component to allow possession
class Possession : Component {
  public:

    // Make this component scriptable
    static void registerPossessionType(sol::environment& env) {

      // Register the usual assign, has, remove functions to Entity
      Script::registerComponentToEntity<Possession>(env, "Possession");

      // Create the Possession user type
      env.new_usertype<Possession>("Possession");
    }

    // Constructor
    Possession(ECS::Entity* e) 
    : Component(e)
    , isActive(true){}

    // Whether this component should respond to input
    bool isActive;

    // Shows the debug information to ImGui
    void showDebugInformation() {
      ImGui::NextColumn();
      ImGui::Text("Possessed: %s", isActive ? "true": "false");
      ImGui::PushItemWidth(-1);
      ImGui::PopItemWidth();
      ImGui::NextColumn();
    }
};

#endif
