// ControlSystem.h
// System to control possessed entities in the scene

#ifndef CONTROLSYSTEM_H
#define CONTROLSYSTEM_H

#include <map>

#include "Game.h"
#include "Scripting.h"

#include "Sprite.h"
#include "Possession.h"
#include "RigidBody.h"
#include "Movement.h"
#include "Abilities.h"

// System that manipulates possessed entities
class ControlSystem : public ECS::EntitySystem {
  public:

    // Register Control System in the world
    static void registerControlSystemFunctions(sol::environment& env, ECS::World* world);

    // Handle input and store meaningful actions
    static void handleInput(const sf::Event& ev);

    // Constructors
    ControlSystem();
    ~ControlSystem();

    // Manipulate all possessed components using their data
    virtual void update(ECS::World* world, const sf::Time& dt) override;

  private:

    // How to describe each keypress
    enum Status {
      DEFAULT,
      PRESSED,
      HELD,
      RELEASED
    };

    // Statuses of all keys on keyboard
    static std::map<sf::Keyboard::Key, ControlSystem::Status> keypresses_;
    static std::map<sf::Mouse::Button, ControlSystem::Status> mouseclicks_;

    // Demote pressed to held and released to default
    static void degradeButtonStatuses();

    // Get the x and y input axis based on keyboard
    static sf::Vector2f calculateInputAxis();

    // Query keyboard events
    static bool isDown(const sf::Keyboard::Key& key);
    static bool isDown(const sf::Mouse::Button& button);
    static bool isUp(const sf::Keyboard::Key& key);
    static bool isUp(const sf::Mouse::Button& button);
    static bool wasPressed(const sf::Keyboard::Key& key);
    static bool wasPressed(const sf::Mouse::Button& button);
    static bool wasReleased(const sf::Keyboard::Key& key);
    static bool wasReleased(const sf::Mouse::Button& button);
};

#endif
