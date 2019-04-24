// ControlSystem.cpp
// System to control possessed entities in the scene

#include "ControlSystem.h"

// Initialise static variables
std::map<sf::Keyboard::Key, ControlSystem::Status> ControlSystem::keypresses_;
std::map<sf::Mouse::Button, ControlSystem::Status> ControlSystem::mouseclicks_;

// Register Control System in the world
void
ControlSystem::registerControlSystem(sol::environment& env, ECS::World* world) {

  // Create and install control system
  env.set_function("useControlSystem", [&env, world]() { 

    // Debug message
    if (Game::getDebugMode()) {
      Console::log("Initialising Control system..");
    }

    // Create the control system to return to the world
    auto* newCS = new ControlSystem();
    world->registerSystem(newCS);
  });
}

// Constructors
ControlSystem::ControlSystem() {}
ControlSystem::~ControlSystem() {}

// Control all possessed entities
void
ControlSystem::update(ECS::World* world, const sf::Time& dt) {

  // Control the scene
  world->each<Possession>([&](ECS::Entity* e, ECS::ComponentHandle<Possession> p) {

    // If we have a RigidBody and Movement, we can move it
    auto r = e->get<RigidBody>();
    auto m = e->get<Movement>();
    if (r.isValid() && m.isValid()) {

      // Query if we're sprinting
      bool isSprinting = isDown(sf::Keyboard::LShift);

      // Assume we are just on the ground
      // Calculate max speeds
      const sf::Vector2f currentSpeed = r->getLinearVelocity();
      sf::Vector2f maxSpeed;
      m->isSprinting = m->stats.canSprint && isSprinting;
      maxSpeed.x = (m->isSprinting ? m->stats.sprintSpeedMult : 1.f) * m->stats.movementSpeed;
      maxSpeed.y = (m->isSprinting && m->stats.canSprintWhileFlying ? m->stats.sprintSpeedMult : 1.f) * 
        (m->stats.canFly ? m->stats.flightSpeed : 0.f);
      sf::Vector2f impulse;

      // Calculate the input axis
      sf::Vector2f inputAxis = calculateInputAxis();

      // Use difference in current and max speed to find speed
      int dir = (inputAxis.x > 0) * 2 - 1;
      if (inputAxis.x != 0.f) {
        impulse.x = ((dir * maxSpeed.x) - currentSpeed.x) * abs(inputAxis.x);

        // Set the animation of the sprite to walk
        auto s = e->get<Sprite>();
        if (s.isValid()) {
          s->flipX = inputAxis.x < 0;
          s->playAnimation("walk");
        }
      }
      else if (r->getIsOnGround()) {
        impulse.x = -currentSpeed.x * 0.1f;

        // Set the animation of the sprite to idle
        auto s = e->get<Sprite>();
        if (s.isValid()) {
          s->playAnimation("idle");
        }
      }

      // Do same for Y, taking flight into account
      dir = (inputAxis.y > 0) * 2 - 1;
      if (inputAxis.y != 0.f && m->stats.canFly) {
        impulse.y = ((dir * maxSpeed.y) - currentSpeed.y) * abs(inputAxis.y);
      }

      // Apply the movement speed
      const float t = dt.asSeconds() * 100;
      r->applyImpulseToCentreVec(impulse * t);

      // Query as to whether we're jumping
      bool isJumping = wasPressed(sf::Keyboard::Space);

      // If on the ground and if desired, jump
      if (m->stats.canJump && isJumping && r->getIsOnGround()) {
        float jump = r->getMass() * -500;
        r->applyImpulseToCentre(0, jump);
      }
    }

    // If we have an Abilities component, we can cast spells
    auto s = e->get<Abilities>();
    if (s.isValid()) {

      // Cast primary and secondary spells
      if (wasPressed(sf::Mouse::Left)) { s->castSpell(0); }
      if (wasReleased(sf::Mouse::Left)) { s->releaseSpell(0); }

      if (wasPressed(sf::Mouse::Right)) { s->castSpell(1); }
      if (wasReleased(sf::Mouse::Right)) { s->releaseSpell(1); }

      // Cast other spells
      if (wasPressed(sf::Keyboard::Key::Num1)) { s->castSpell(2); }
      if (wasReleased(sf::Keyboard::Key::Num1)) { s->releaseSpell(2); }
      if (wasPressed(sf::Keyboard::Key::Num2)) { s->castSpell(3); }
      if (wasReleased(sf::Keyboard::Key::Num2)) { s->releaseSpell(3); }
      if (wasPressed(sf::Keyboard::Key::Num3)) { s->castSpell(4); }
      if (wasReleased(sf::Keyboard::Key::Num3)) { s->releaseSpell(4); }
    }
  });

  // After consuming any 'on press' commands, degrade button statuses
  degradeButtonStatuses();
}

// Handle input from the game
void
ControlSystem::handleInput(const sf::Event& ev) {

  // Handle keypresses
  if (ev.type == sf::Event::KeyPressed || ev.type == sf::Event::KeyReleased) {

    // Get the current status of the key
    Status active = (ev.type == sf::Event::KeyPressed) ? Status::PRESSED : Status::RELEASED;
    Status passive = (active == Status::PRESSED) ? Status::HELD : Status::DEFAULT;

    // See if we've pressed this key previously
    auto it = keypresses_.find(ev.key.code);
    if (it != keypresses_.end()) {

      // Set the status
      if (it->second != passive) { 
        it->second = active; 
      }
    }
    else {
      keypresses_[ev.key.code] = Status::DEFAULT;
    }
  }

  // Handle keypresses
  else if (ev.type == sf::Event::MouseButtonPressed || ev.type == sf::Event::MouseButtonReleased) {

    // Get the current status of the key
    Status active = (ev.type == sf::Event::MouseButtonPressed) ? Status::PRESSED : Status::RELEASED;

    // See if we've pressed this key previously
    auto it = mouseclicks_.find(ev.mouseButton.button);
    if (it != mouseclicks_.end()) {

      // Set the status
      it->second = active;
    }
    else {
      mouseclicks_[ev.mouseButton.button] = Status::DEFAULT;
    }
  }
}

// Demote pressed to held and released to default
void 
ControlSystem::degradeButtonStatuses() {

  // Degrade keypresses
  for (auto i = keypresses_.begin(); i != keypresses_.end(); ++i) {
    if (i->second == Status::PRESSED) { i->second = Status::HELD; }
    else if (i->second == Status::RELEASED) { i->second = Status::DEFAULT; }
  }

  // Degrade mouse button presses
  for (auto i = mouseclicks_.begin(); i != mouseclicks_.end(); ++i) {
    if (i->second == Status::PRESSED) { i->second = Status::HELD; }
    else if (i->second == Status::RELEASED) { i->second = Status::DEFAULT; }
  }
}

// Get the x and y input axis based on keyboard
sf::Vector2f
ControlSystem::calculateInputAxis() {

  // Query keyboard
  bool up = isDown(sf::Keyboard::Key::W);
  bool left = isDown(sf::Keyboard::Key::A);
  bool down = isDown(sf::Keyboard::Key::S);
  bool right = isDown(sf::Keyboard::Key::D);

  // Create and return input vectors
  sf::Vector2f input;
  if (up) { input.y -= 1.f; }
  if (down) { input.y += 1.f; }
  if (left) { input.x -= 1.f; }
  if (right) { input.x += 1.f; }
  return input;
}

// Query if a key is pressed or held
bool 
ControlSystem::isDown(const sf::Keyboard::Key& key) {
  auto it = keypresses_.find(key);
  if (it != keypresses_.end()) {
    return it->second == Status::PRESSED || it->second == Status::HELD;
  }
  else {
    keypresses_[key] = Status::DEFAULT;
  }
  return false;
}
bool 
ControlSystem::isDown(const sf::Mouse::Button& button) {
  auto it = mouseclicks_.find(button);
  if (it != mouseclicks_.end()) {
    return it->second == Status::PRESSED || it->second == Status::HELD;
  }
  else {
    mouseclicks_[button] = Status::DEFAULT;
  }
  return false;
}

// Query if a key is not pressed
bool
ControlSystem::isUp(const sf::Keyboard::Key& key) {
  auto it = keypresses_.find(key);
  if (it != keypresses_.end()) {
    return it->second == Status::RELEASED || it->second == Status::DEFAULT;
  }
  else {
    keypresses_[key] = Status::DEFAULT;
  }
  return true;
}
bool 
ControlSystem::isUp(const sf::Mouse::Button& button) {
  auto it = mouseclicks_.find(button);
  if (it != mouseclicks_.end()) {
    return it->second == Status::RELEASED || it->second == Status::DEFAULT;
  }
  else {
    mouseclicks_[button] = Status::DEFAULT;
  }
  return true;
}

// Query if a key was just pressed
bool
ControlSystem::wasPressed(const sf::Keyboard::Key& key) {
  auto it = keypresses_.find(key);
  if (it != keypresses_.end()) {
    return it->second == Status::PRESSED;
  }
  else {
    keypresses_[key] = Status::DEFAULT;
  }
  return false;
}
bool 
ControlSystem::wasPressed(const sf::Mouse::Button& button) {
  auto it = mouseclicks_.find(button);
  if (it != mouseclicks_.end()) {
    return it->second == Status::PRESSED;
  }
  else {
    mouseclicks_[button] = Status::DEFAULT;
  }
  return false;
}

// Query if a key was just released
bool
ControlSystem::wasReleased(const sf::Keyboard::Key& key) {
  auto it = keypresses_.find(key);
  if (it != keypresses_.end()) {
    return it->second == Status::RELEASED;
  }
  else {
    keypresses_[key] = Status::DEFAULT;
  }
  return false;
}
bool 
ControlSystem::wasReleased(const sf::Mouse::Button& button) {
  auto it = mouseclicks_.find(button);
  if (it != mouseclicks_.end()) {
    return it->second == Status::RELEASED;
  }
  else {
    mouseclicks_[button] = Status::DEFAULT;
  }
  return false;
}
