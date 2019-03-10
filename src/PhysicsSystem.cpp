// PhysicsSystem.cpp
// System to simulate physics in game

#include "PhysicsSystem.h"

// Constructor
PhysicsSystem::PhysicsSystem() 
  : world_(b2Vec2(0.0f, 10.0f))
  , timeStepAccumilator_(0.0f) {
  
  // Draw physics
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

  if (steps > 0) {
    timeStepAccumilator_ -= steps * fixedTimeStep_;
  }

  // Simulate only when we should
  fixedTimeStepRatio_ = timeStepAccumilator_ / fixedTimeStep_;
  const int stepsClamped = std::min(steps, maxSteps_);
  for (int i = 0; i < stepsClamped; ++i) {

    // Reset smoothing states of all entities
    world->each<Transform, RigidBody>([&](ECS::Entity* e, ECS::ComponentHandle<Transform> t, ECS::ComponentHandle<RigidBody> r) {
      resetSmoothStates(e, t, r);
    });

    // Simulate
    singleStep(fixedTimeStep_);
  }

  // Reset applied forces
  world_.ClearForces();

  // Create rigidbodies on entities that need them and tween
  world->each<Transform, RigidBody>([&](ECS::Entity* e, ECS::ComponentHandle<Transform> t, ECS::ComponentHandle<RigidBody> r) {

    // Create rigidbodies 
    ensureRigidBody(r, t);

    // Interpolate between states
    smoothState(r);
  });

  // Render the physics system
  // @TODO: Move this to a render system
  world_.DrawDebugData();
}

// Single-step the physics
void
PhysicsSystem::singleStep(float timeStep) {
  world_.Step(timeStep, velocityIterations_, positionIterations_);
}

// Interpolation between frames
void
PhysicsSystem::smoothState(ECS::ComponentHandle<RigidBody> rb) {

  // Ensure there's a body to work with
  b2Body* const body = rb->getBody();
  if (body != nullptr) {

    // Set up variables
    const float oneMinusRatio = 1.0f - fixedTimeStepRatio_;

    // Interpolate based on previous position
    if (body->GetType() != b2_staticBody) {

      rb->smoothedPosition_ =
        fixedTimeStepRatio_ * body->GetPosition() +
        oneMinusRatio * rb->previousPosition_;

      printf("Smoothed position: %f, %f\n", body->GetPosition().x, body->GetPosition().y);

      rb->smoothedAngle_ = 
        body->GetAngle() +
        oneMinusRatio * rb->previousAngle_;
    }
  }
}

// When the step occurs, reset any smoothing
void
PhysicsSystem::resetSmoothStates(ECS::Entity* ent, ECS::ComponentHandle<Transform> t, ECS::ComponentHandle<RigidBody> r) {

  // Set up variables
  b2Body* body = r->getBody();

  // Reset any smoothing
  if (body->GetType() != b2_staticBody) {
    r->smoothedPosition_ = r->previousPosition_ = body->GetPosition();
    r->smoothedAngle_ = r->previousAngle_ = body->GetAngle();
  }
}

// Creates rigidbodies as needed
void
PhysicsSystem::ensureRigidBody(ECS::ComponentHandle<RigidBody> r, ECS::ComponentHandle<Transform> t) {

  // Create rigidbodies if they don't exist
  if (r->getBody() == nullptr) {

    printf("Creating rigidbody..\n");

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
    printf("Added %d fixture(s) to Rigidbody\n", count);

    if (body != nullptr)
      printf("Rigidbody created!\n");
    else
      printf("Rigidbody failed!\n");
  }
}

// Convert to SFML vectors
sf::Vector2i 
PhysicsSystem::convertToSF(const b2Vec2& vec) {
  return sf::Vector2i(std::round(vec.x * scale_), std::round(vec.y * scale_));
}

// Convert to Box2D vectors
b2Vec2 
PhysicsSystem::convertToB2(const sf::Vector2i& vec) {
  return b2Vec2(vec.x / scale_, vec.y / scale_);
}
