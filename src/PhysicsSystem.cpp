// PhysicsSystem.cpp
// System to simulate physics in game

#include "PhysicsSystem.h"

// Register a physics system in this world
void
PhysicsSystem::registerPhysicsSystemFunctions(ECS::World* world) {

  // Debug message
  if (Game::getDebugMode()) {
    printf("Initialising Physics\n");
  }

  // Create and install physics system
  Game::lua.set_function("usePhysicsSystem", [world]() { 
    auto* newPS = new PhysicsSystem();
    world->registerSystem(newPS);

    // Register functions to address this system
    Game::lua.set_function("getGravity", &PhysicsSystem::getGravity, newPS);
    Game::lua.set_function("setGravity", &PhysicsSystem::setGravity, newPS);
    Game::lua.set_function("setGravityMult", &PhysicsSystem::setGravityMult, newPS);

    return newPS;
  });
}

// Constructor
PhysicsSystem::PhysicsSystem() 
  : defaultGravity_(sf::Vector2f(0.f, 10.f))
  , world_(b2Vec2(defaultGravity_.x, defaultGravity_.y))
  , timeStepAccumilator_(0.0f) {

  // Ensure that the debug system's scale matches this system
  PhysicsDebugDraw::scale = scale_;
  
  // Set up the drawing of physics
	physicsDebugDraw_.SetFlags(PhysicsDebugDraw::e_shapeBit);
	world_.SetDebugDraw(&physicsDebugDraw_);
}

// Handle the physics independant to game FPS
void 
PhysicsSystem::update(ECS::World* world, const sf::Time& dt) {

  // Accumilate time and calculate steps
  timeStepAccumilator_ += dt.asSeconds();
  const auto steps = static_cast<int>(
    std::floor(timeStepAccumilator_ / fixedTimeStep_));

  // Calculate if we should simulate
  if (steps > 0) {
    timeStepAccumilator_ -= steps * fixedTimeStep_;
  }
  fixedTimeStepRatio_ = timeStepAccumilator_ / fixedTimeStep_;
  const int stepsClamped = std::min(steps, maxSteps_);

  // Simulate only when we should
  for (int i = 0; i < stepsClamped; ++i) {

    // Reset smoothing states of all entities
    world->each<Transform, RigidBody>([&](ECS::Entity* e, ECS::ComponentHandle<Transform> t, ECS::ComponentHandle<RigidBody> r) {
      resetSmoothStates(t, r);
    });

    // Simulate
    singleStep(fixedTimeStep_);
  }

  // Reset applied forces
  world_.ClearForces();

  // Create rigidbodies on entities that need them and tween
  world->each<Transform, RigidBody>([&](ECS::Entity* e, ECS::ComponentHandle<Transform> t, ECS::ComponentHandle<RigidBody> r) {
    ensureRigidBody(t, r);
    smoothState(t, r);
  });
}

// Single-step the physics
void
PhysicsSystem::singleStep(float timeStep) {
  world_.Step(timeStep, velocityIterations_, positionIterations_);
}

// Interpolation between frames
void
PhysicsSystem::smoothState(ECS::ComponentHandle<Transform> t, ECS::ComponentHandle<RigidBody> rb) {

  // Ensure there's a body to work with
  b2Body* const body = rb->getBody();
  if (body != nullptr) {

    // Set up variables
    const float oneMinusRatio = 1.0f - fixedTimeStepRatio_;

    // Interpolate based on previous position
    if (body->GetType() != b2_staticBody) {

      // Tween position
      t->position = convertToSF(
        fixedTimeStepRatio_ * body->GetPosition() +
        oneMinusRatio * rb->previousPosition_);

      // Tween rotation
      t->rotation = body->GetAngle() +
        oneMinusRatio * rb->previousAngle_;
    }
  }
}

// When the step occurs, reset any smoothing
void
PhysicsSystem::resetSmoothStates(ECS::ComponentHandle<Transform> t, ECS::ComponentHandle<RigidBody> r) {

  // Set up variables
  b2Body* body = r->getBody();

  // Reset any smoothing
  if (body != nullptr && body->GetType() != b2_staticBody) {
    r->previousPosition_ = body->GetPosition();
    r->previousAngle_ = body->GetAngle();
  }
}

// Creates rigidbodies as needed
void
PhysicsSystem::ensureRigidBody(ECS::ComponentHandle<Transform> t, ECS::ComponentHandle<RigidBody> r) {

  // Create rigidbodies if they don't exist
  if (r->getBody() == nullptr) {

    // Make the rigidbody
    b2Body* body = world_.CreateBody(&r->getBodyDef());
    r->setBody(body);

    // Initialise the body with correct transforms
    body->SetTransform (convertToB2(t->position), t->rotation);

    // Create any fixtures
    const std::vector<b2FixtureDef>& fixtureDefs_ = r->getFixtureDefs();
    int count = 0;
    for (auto& fixture : fixtureDefs_) {
      body->CreateFixture(&fixture);
      count++;
    }
  }
}

// Get gravity
sf::Vector2f
PhysicsSystem::getGravity() const {
  return convertToSF(world_.GetGravity());
}

// Set gravity multiplier
void
PhysicsSystem::setGravityMult(float m) {
  const auto g = getGravity();
  setGravity(g.x * m, g.y * m);
}

// Set gravity
void
PhysicsSystem::setGravity(float gx, float gy) {
  world_.SetGravity(convertToB2(sf::Vector2f(gx, gy)));
}

// Convert to SFML vectors
sf::Vector2f 
PhysicsSystem::convertToSF(const b2Vec2& vec) const {
  return sf::Vector2f(vec.x * scale_, vec.y * scale_);
}

// Convert to Box2D vectors
b2Vec2 
PhysicsSystem::convertToB2(const sf::Vector2f& vec) const {
  return b2Vec2(vec.x / scale_, vec.y / scale_);
}

// Render the physics in debug mode
void
PhysicsSystem::receive(ECS::World* w, const DebugRenderPhysicsEvent& e) {

  // Save the pointer to the window
  physicsDebugDraw_.window = &e.window;

  // Render the physics system
  world_.DrawDebugData();
}
