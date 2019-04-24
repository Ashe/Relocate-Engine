// Stats.h
// A component containing base stats for an entity

#ifndef STATS_H
#define STATS_H

#include "Game.h"
#include "Scripting.h"

#include "Movement.h"
#include "Combat.h"

// Base stats
class Stats : Component {
  public:

    // Make this component scriptable
    static void registerStatsType(sol::environment& env) {

      // Register the usual assign, has, remove functions to Entity
      Script::registerComponentToEntity<Stats>(env, "Stats");

      // Create the Stats usertype
      env.new_usertype<Stats>("Stats",
        "movement", &Stats::moveStats,
        "combat", &Stats::combatStats
      );
    }

    // Constructor
    Stats(ECS::Entity* e)
      : Component(e)
      , moveStats(MovementStats()) {
    }

    // Stat modules
    MovementStats moveStats;
    CombatStats combatStats;

    // Shows the debug information to ImGui
    void showDebugInformation() {
      ImGui::NextColumn();
      combatStats.showDebugInformation();
      moveStats.showDebugInformation();
      ImGui::PushItemWidth(-1);
      ImGui::PopItemWidth();
      ImGui::NextColumn();
    }
};

#endif
