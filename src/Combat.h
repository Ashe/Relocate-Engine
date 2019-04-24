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
        "stats", &Combat::stats,
        "currentHealth", sol::property(&Combat::getCurrentHealth),
        "resetHealth", &Combat::resetHealthToFull,
        "dealImpactDamage", &Combat::dealImpactDamage
      );
    }

    // Constructor
    Combat(ECS::Entity* e)
      : Component(e) {
    }

    // Combat stats
    CombatStats stats;

    // Get current health
    unsigned getCurrentHealth() const {
      return currentHealth_;
    }

    // Go back to full health
    unsigned resetHealthToFull() {
      currentHealth_ = stats.maxHealth;
      return currentHealth_;
    }

    // Deal damage via fall damage or collision boxes
    unsigned dealImpactDamage(double impact) {
      impact -= 10.f;
      if (impact > 0.f) {
        const unsigned damage = ceil(impact);
        currentHealth_ -= damage;
        Console::log("Ouch! You took %f (%u) damage!", impact, damage);
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
    unsigned currentHealth_;

};


#endif
