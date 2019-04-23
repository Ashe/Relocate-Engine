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

    // Write movement stats
    if (!e->has<Movement>()) { e->assign<Movement>(e); }
    auto movement = e->get<Movement>();
    writeMovementStats(s, movement);
  });
}


// Write stats to the movement component such as move speed
void 
StatSystem::writeMovementStats(const ECS::ComponentHandle<Stats>& s, ECS::ComponentHandle<Movement>& m) {

  // Copy the stats over 
  // @TODO: Change the stats with buffs and equipment
  m->stats = s->moveStats;
}

