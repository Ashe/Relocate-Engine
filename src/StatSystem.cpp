// StatSystem.cpp
// System for transferring stats into relevant components

#include "StatSystem.h"

// Register this system in the world
void 
StatSystem::registerStatSystem(sol::environment& env, ECS::World* world) {

  // Create and install stat system
  env.set_function("useStatSystem", [&env, world]() { 

    // Debug message
    Console::log("Initialising Stat System..");

    // Create the camera system to return to the world
    auto* newSTS = new StatSystem();
    world->registerSystem(newSTS);
  });
}

// Write to appropriate components
void 
StatSystem::update(ECS::World* world, const sf::Time& dt) {

  // Get every entity with a sprite and transform
  world->each<Stats> ([&](ECS::Entity* e, ECS::ComponentHandle<Stats> s) {

    // Get stats component
    const Stats& stats = s.get();

    // Write movement stats
    bool has = e->has<Movement>();
    if (!has) { e->assign<Movement>(e); }
    auto movement = e->get<Movement>();
    writeMovementStats(stats, movement.get(), has);

    // Write combat stats
    has = e->has<Combat>();
    if (!has) { e->assign<Combat>(e); }
    auto combat = e->get<Combat>();
    writeCombatStats(stats, combat.get(), has);

  });
}


// Write stats to the movement component such as move speed
void 
StatSystem::writeMovementStats(const Stats& s, Movement& m, bool first) {

  // Copy the stats over 
  // @TODO: Change the stats with buffs and equipment
  m.stats = s.moveStats;
}

// Write stats to the combat component such as max health
void 
StatSystem::writeCombatStats(const Stats& s, Combat& c, bool first) {

  // Copy stats
  // @TODO: Change the stats with buffs and equipment
  c.stats = s.combatStats;

  // If the component is just being created, set the current health to max health
  if (first) { c.resetHealthToFull(); }
}
