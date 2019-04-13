// Possession.h
// A component signalling whether this entity is possessed by the player

#ifndef POSSESSION_H
#define POSSESSION_H

#include "Game.h"
#include "Scripting.h"

// Component to allow possession
class Possession {
  public:

    // Make this component scriptable
    static void registerPossessionType() {

      // Register the usual assign, has, remove functions to Entity
      Script::registerComponentToEntity<Possession>("Possession");

      // Create the Possession user type
      Game::lua.new_usertype<Possession>("Possession");
    }

    // Constructor
    Possession() 
    : isActive(true){}

    // Whether this component should respond to input
    bool isActive;
};

#endif
