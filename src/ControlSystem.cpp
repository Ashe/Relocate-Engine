// ControlSystem.cpp
// System to control possessed entities in the scene

#include "ControlSystem.h"

// Initialise static variables
sf::Vector2f ControlSystem::inputAxis = sf::Vector2f();
bool ControlSystem::isSprinting = false;
bool ControlSystem::isJumping = false;

// Register Control System in the world
void
ControlSystem::registerControlSystemFunctions(sol::environment& env, ECS::World* world) {

  // Create and install physics system
  env.set_function("useControlSystem", [&env, world]() { 

    // Debug message
    if (Game::getDebugMode()) {
      printf("Initialising Control system..\n");
    }

    // Create the physics system to return to the world
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

      // Assume we are just on the ground
      // Calculate max speeds
      const sf::Vector2f currentSpeed = r->getLinearVelocity();
      sf::Vector2f maxSpeed;
      maxSpeed.x = (m->canSprint && isSprinting ? m->sprintSpeedMult : 1.f) * m->movementSpeed;
      maxSpeed.y = (m->canSprint && m->canSprintWhileFlying && isSprinting ? m->sprintSpeedMult : 1.f) * 
        (m->canFly ? m->flightSpeed : 0.f);
      sf::Vector2f impulse;

      // Use difference in current and max speed to find speed
      int dir = (inputAxis.x > 0) * 2 - 1;
      if (inputAxis.x != 0.f) {
        impulse.x = ((dir * maxSpeed.x) - currentSpeed.x) * abs(inputAxis.x);
      }
      else {
        impulse.x = -currentSpeed.x * 0.1f;
      }

      // Do same for Y, taking flight into account
      dir = (inputAxis.y > 0) * 2 - 1;
      if (inputAxis.y != 0.f && m->canFly) {
        impulse.y = ((dir * maxSpeed.y) - currentSpeed.y) * abs(inputAxis.y);
      }

      // Apply the movement speed
      const float t = dt.asSeconds() * 100;
      r->applyImpulseToCentreVec(impulse * t);

      // If on the ground and if desired, jump
      if (m->canJump && isJumping && r->getIsOnGround()) {
        r->applyImpulseToCentre(0, r->getMass() * -0.2);
      }
    }
  });
}

// Handle input from the game
void
ControlSystem::handleInput(const sf::Event& ev) {

  // Control the scene
  if (ev.type == sf::Event::KeyPressed || ev.type == sf::Event::KeyReleased) {
    const int inv = ev.type == sf::Event::KeyPressed ? 1 : -1;

    // Handle x-axis
    if (ev.key.code == sf::Keyboard::A) {
      inputAxis.x -= 1.f * inv;
    }
    if (ev.key.code == sf::Keyboard::D) {
      inputAxis.x += 1.f * inv;
    }
    if (inputAxis.x > 1) inputAxis.x = 1;
    if (inputAxis.x < -1) inputAxis.x = -1;

    // Handle y-axis
    if (ev.key.code == sf::Keyboard::W || ev.key.code == sf::Keyboard::Space) {
      inputAxis.y -= 1.f * inv;
    }
    if (ev.key.code == sf::Keyboard::S) {
      inputAxis.y += 1.f * inv;
    }
    if (inputAxis.y > 1) inputAxis.y = 1;
    if (inputAxis.y < -1) inputAxis.y = -1;

    // Handle sprinting
    if (ev.key.code == sf::Keyboard::LShift || ev.key.code == sf::Keyboard::RShift) {
      isSprinting = ev.type == sf::Event::KeyPressed;
    }

    // Handle jumping
    if (ev.key.code == sf::Keyboard::Space) {
      isJumping = ev.type == sf::Event::KeyPressed;
    }
  }
}
