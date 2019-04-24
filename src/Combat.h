// Combat.h
// Component for use in combat

#ifndef COMBAT_H
#define COMBAT_H

#include "Game.h"
#include "Scripting.h"

// Stats for combat
struct CombatStats {
  unsigned maxHealth = 1;

  void showDebugInformation() {
    ImGui::Text("Max health: %u", maxHealth);
  }
};

// Manage health and other combat-related values
class Combat : Component {
  public:

    // Make this component scriptable
    static void registerCombatType(sol::environment& env) {

      // Register the usual assign, has, remove functions to Entity
      Script::registerComponentToEntity<Combat>(env, "Combat");

      // Create the stats usertype
      env.new_usertype<CombatStats>("CombatStats",
        sol::constructors<CombatStats()>(),
        "maxHealth", &CombatStats::maxHealth
      );

      // Create the Combat usertype
      env.new_usertype<Combat>("Combat",
        "currentHealth", &Combat::currentHealth,
        "stats", &Combat::stats
      );
    }

    // Constructor
    Combat(ECS::Entity* e)
      : Component(e) {
    }

    // Combat stats
    CombatStats stats;

    // Current health
    unsigned currentHealth;

    // Shows the debug information to ImGui
    void showDebugInformation() {
      ImGui::NextColumn();
      ImGui::Text("Current health: %u", currentHealth);
      stats.showDebugInformation();
      ImGui::PushItemWidth(-1);
      ImGui::PopItemWidth();
      ImGui::NextColumn();
    }
};


#endif
