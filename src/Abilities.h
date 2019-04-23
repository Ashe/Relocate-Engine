// Abilities.h
// A component for storing how an entity can fight

#ifndef ABILITIES_H
#define ABILITIES_H

#include <map>

#include "Game.h"
#include "Scripting.h"

#include "ResourceManager.h"
#include "Spell.h"

// Container of abilities and spells
class Abilities {
  public:

    // Make this component scriptable
    static void registerAbilitiesType(sol::environment& env) {

      // Register the usual assign, has, remove functions to Entity
      Script::registerComponentToEntity<Abilities>(env, "Abilities");

      // Create the Abilities usertype
      env.new_usertype<Abilities>("Abilities",
        "addAbility", &Abilities::addAbilityFromResources,
        "castSpell", &Abilities::castSpell,
        "releaseSpell", &Abilities::releaseSpell
      );
    }

    // Constructor
    Abilities() {}

    // Adds a spell or ability to the map
    bool addAbility(unsigned slot, Spell spell) {
      spells_[slot] = spell;
      return true;
    }

    // Adds a spell by name
    bool addAbilityFromResources(unsigned slot, const std::string& name) {

      // Easy outs
      if (name == "") { return false; }

      // Attempts to get the resource
      Resource& resource = ResourceManager::getResource(name);
      if (resource.getType() != Resource::Type::SPELL) { 
        Console::log("[Error] Could not add spell: %s\nNonexistant or incorrect resource type.", name.c_str());
        return false; 
      }

      // Get texture from resource
      Spell* spell = (Spell*)resource.get();

      // Ensure the spell is okay
      if (spell == nullptr) { 
        Console::log("[Error] Could not add spell: %s\nResource is NULL.", name.c_str());
        return false; 
      }

      // Save the spell
      spells_[slot] = *spell;
      return true;
    }

    // Casts a given spell in a slot
    bool castSpell(unsigned slot) {
      auto* spell = getSpell(slot);
      if (spell != nullptr) {
        spell->cast();
        return true;
      }
      return false;
    }

    // Releases a given spell in a slot
    bool releaseSpell(unsigned slot) {
      auto* spell = getSpell(slot);
      if (spell != nullptr) {
        spell->release();
        return true;
      }
      return false;
    }

    // Passively casts a given spell in a slot
    void updateAllSpells(const sf::Time& dt) {
      for (auto i = spells_.begin(); i != spells_.end(); ++i) {
        i->second.passive(dt);
      }
    }

    // Find's a spell in the map
    Spell* getSpell(unsigned slot) {

      // Try to find the spell in the map
      auto it = spells_.find(slot);
      if (it != spells_.end()) {
        return &it->second;
      }

      // Otherwise return nullptr
      return nullptr;
    }

  private:

    // Spells this entity can cast
    std::map<unsigned, Spell> spells_;

};

#endif
