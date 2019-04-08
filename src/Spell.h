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
    static void registerFunctions() {

      // Register the spell type
      Game::lua.new_usertype<Spell>("Spell",
        sol::constructors<Spell(const std::string&)>(),
        // Casting functions
        "castMajor", &Spell::castMajor,
        "castMinor", &Spell::castMinor,
        "releaseMajor", &Spell::releaseMajor,
        "releaseMinor", &Spell::releaseMinor,
        // Variables
        "name", &Spell::name_,
        "onCastMajor", &Spell::onCastMajor_,
        "onCastMinor", &Spell::onCastMinor_,
        "onReleaseMajor", &Spell::onReleaseMajor_,
        "onReleaseMinor", &Spell::onReleaseMinor_
      );
    }

    // Constructors 
    Spell(const std::string& name)
    : name_(name) {}

    // Major component
    void castMajor() { cast(onCastMajor_); }
    void releaseMajor() {cast(onReleaseMajor_); }

    // Minor component
    void castMinor() { cast(onCastMinor_); }
    void releaseMinor() { cast(onReleaseMinor_); }

  private:

    // Cast a spell comopnent
    void cast(sol::protected_function& spell) {

      // If the script is valid, try to run it
      if (spell.valid()) {
        auto attempt = spell(this);
        if (!attempt.valid()) {
          sol::error err = attempt;
          Console::log("[Error] in Spell '%s':\n> %s", 
            name_.c_str(), err.what());
        }
      }
    }

    // Name of the spell
    const std::string name_;

    // Main spell cast and release functions
    sol::protected_function onCastMajor_;
    sol::protected_function onCastMinor_;
    sol::protected_function onReleaseMajor_;
    sol::protected_function onReleaseMinor_;

};

#endif
