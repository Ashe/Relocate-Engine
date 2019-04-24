// Combat.h
// Component for use in combat

#ifndef COMBAT_H
#define COMBAT_H

#include "Game.h"
#include "Scripting.h"

// Stats for combat
struct CombatStats {
  int maxHealth = 1;
  bool deleteImmediatelyOnDeath = false;
  bool deleteAfterDeathAnimation = true;

  void showDebugInformation() {
    ImGui::Text("Max health: %u", maxHealth);
    ImGui::Text("Delete immediately on death: %s", deleteImmediatelyOnDeath ? "true" : "false" );
    ImGui::Text("Delete after death animation: %s", deleteAfterDeathAnimation ? "true" : "false" );
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
        "maxHealth", &CombatStats::maxHealth,
        "deleteImmediately", &CombatStats::deleteImmediatelyOnDeath,
        "deleteAfterAnimation", &CombatStats::deleteAfterDeathAnimation
      );

      // Create the Combat usertype
      env.new_usertype<Combat>("Combat",
        "stats", &Combat::stats,
        "currentHealth", sol::property(&Combat::getCurrentHealth),
        "resetHealth", &Combat::resetHealthToFull,
        "dealImpactDamage", &Combat::dealImpactDamage
      );
    }

    // Constructor
    Combat(ECS::Entity* e)
      : Component(e) 
      , currentHealth_(1) {
    }

    // Combat stats
    CombatStats stats;

    // Get current health
    int getCurrentHealth() const {
      return currentHealth_;
    }

    // Go back to full health
    int resetHealthToFull() {
      currentHealth_ = stats.maxHealth;
      return currentHealth_;
    }

    // Deal damage via fall damage or collision boxes
    int dealImpactDamage(double impact) {
      impact -= 10.f;
      if (impact > 0.f) {
        const int damage = ceil(impact);
        currentHealth_ -= damage;
      }
      return currentHealth_;
    }

    // Shows the debug information to ImGui
    void showDebugInformation() {
      ImGui::NextColumn();
      ImGui::Text("Current health: %u", currentHealth_);
      stats.showDebugInformation();
      ImGui::PushItemWidth(-1);
      ImGui::PopItemWidth();
      ImGui::NextColumn();
    }

  private:

    // Current health
    int currentHealth_;

};


#endif
