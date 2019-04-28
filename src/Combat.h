// Combat.h
// Component for use in combat

#ifndef COMBAT_H
#define COMBAT_H

#include "Game.h"
#include "Scripting.h"

// Used in damage numbers
#include "Transform.h"
#include "Expire.h"
#include "Text.h"

// Stats for combat
struct CombatStats {
  int maxHealth = 1;
  float deathDelay = 0.f;
  bool deleteOnDeath = true;
  bool deleteAfterAnimation = true;

  void showDebugInformation() {
    ImGui::Text("Max health: %u", maxHealth);
    ImGui::Text("Delete on death: %s", deleteOnDeath ? "true" : "false" );
    ImGui::Text("Delete after death animation: %s", deleteAfterAnimation ? "true" : "false" );
    ImGui::Text("Death delay: %f", deathDelay);
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
        "deathDelay", &CombatStats::deathDelay,
        "deleteOnDeath", &CombatStats::deleteOnDeath,
        "deleteAfterAnimation", &CombatStats::deleteAfterAnimation
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
      impact -= 15.f;
      if (impact > 0.f) {

        // Perform calculations
        const int damage = ceil(impact);
        currentHealth_ -= damage;

        // Create damage number
        if (owner_->has<Transform>()) {
          auto* world = Game::getWorld();
          if (world != nullptr) {
            auto trans = owner_->get<Transform>();
            auto* e = world->create();
            auto text = e->assign<Text>(e, std::to_string(damage));
            text->setFillColor(sf::Color::Red);
            text->setOutlineColor(sf::Color::White);
            text->centerText();
            e->assign<Transform>(e, trans->position);
            e->assign<Expire>(e, 1.5f);
          }
        }
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
