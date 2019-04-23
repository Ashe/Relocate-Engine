// Spell.h
// Allows for the creation and usage of spells

#ifndef SPELL_H
#define SPELL_H

#include "Game.h"
#include "Scripting.h"

// A spell is used to manipulate the game world in some way
class Spell {
  public:
    
    // Register spell functions to lua
    static void registerSpellType() {

      // Register the spell type
      Game::lua.new_usertype<Spell>("Spell",
        sol::constructors<Spell()>(),
        // Casting functions
        "cast", &Spell::cast,
        "release", &Spell::release,
        "passive", &Spell::passive,
        // Variables
        "name", &Spell::name_,
        "onCast", &Spell::onCast_,
        "onRelease", &Spell::onRelease_,
        "onPassive", &Spell::onPassive_
      );
    }

    // Constructors 
    Spell()
    : name_("unnamed_spell") {}

    // Major component
    void cast(ECS::Entity* const e) { safeCast(onCast_, e); }
    void release(ECS::Entity* const e) { safeCast(onRelease_, e); }

    // Casts every frame
    void passive(ECS::Entity* const e, const sf::Time& dt) { 
      if (onPassive_.valid()) {
        auto attempt = onPassive_(e, dt);
        if (!attempt.valid()) {
          sol::error err = attempt;
          Console::log("[Error] in Spell '%s':\n> %s", 
            name_.c_str(), err.what());
        }
      }
    }

    // Get name of spell
    std::string getName() const { 
      return name_; 
    }

  private:

    // Cast a spell comopnent
    void safeCast(sol::protected_function& spell, ECS::Entity* const e) {

      // If the script is valid, try to run it
      if (spell.valid()) {
        auto attempt = spell(e);
        if (!attempt.valid()) {
          sol::error err = attempt;
          Console::log("[Error] in Spell '%s':\n> %s", 
            name_.c_str(), err.what());
        }
      }
    }

    // Name of the spell
    std::string name_;

    // Main spell cast and release functions
    sol::protected_function onCast_;
    sol::protected_function onPassive_;
    sol::protected_function onRelease_;

};

#endif
