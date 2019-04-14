// PhysicsSystem.cpp
// System to simulate physics in game

#include "PhysicsSystem.h"

// Define statics
const float PhysicsSystem::scale = 100.f;
bool PhysicsSystem::showPhysicsWindow_ = false;
bool PhysicsSystem::showRigidBodies_ = true;

// Register a physics system in this world
void
PhysicsSystem::registerPhysicsSystemFunctions(sol::environment& env, ECS::World* world) {

  // Create and install physics system
  env.set_function("usePhysicsSystem", [&env, world]() { 

    // Debug message
    if (Game::getDebugMode()) {
      Console::log("Initialising Physics system..");
    }

    // Create the physics system to return to the world
    auto* newPS = new PhysicsSystem();
    world->registerSystem(newPS);

    // Get the new physics world
    b2World* physicsWorld = newPS->getWorld();

    // Allow the system's manipulation through lua
    env.set("Physics", newPS);
    env.new_usertype<PhysicsSystem>("PhysicsSystem",
      "gravity", sol::property(
        &PhysicsSystem::getGravity,
        &PhysicsSystem::setGravityVec),
      "setGravityMult", &PhysicsSystem::setGravityMult,
      "bodyCount", sol::property(
        [](const PhysicsSystem& self) { return self.world_.GetBodyCount(); }),
      "showHitboxes", sol::property(
        [](const PhysicsSystem& self) { return PhysicsSystem::showRigidBodies_; },
        [](bool enable) { PhysicsSystem::showRigidBodies_ = enable; })
    );

    // Add global commands to auto complete
    Console::addCommand("[Class] Physics");
    Console::addCommand("Physics.gravity");
    Console::addCommand("Physics:setGravityMult");
    Console::addCommand("Physics.bodyCount");
    Console::addCommand("Physics.showHitboxes");

    // Allow the use of RigidBodies
    RigidBody::registerRigidBodyType(env, physicsWorld);
  });
}

// Constructor, enable debugging of physics
PhysicsSystem::PhysicsSystem() 
  : defaultGravity_(sf::Vector2f(0.f, 1000.f))
  , world_(convertToB2(defaultGravity_))
  , timeStepAccumilator_(0.0f) {

  // Set up our contact listener
  world_.SetContactListener(&contactListener_);

  // Set up the drawing of physics
	physicsDebugDraw_.SetFlags(PhysicsDebugDraw::e_shapeBit);
	world_.SetDebugDraw(&physicsDebugDraw_);
}

// Clean up the world
PhysicsSystem::~PhysicsSystem() {
  //@TODO: Figure out if we need to do something here?
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

  // Give rigidbodies access to their entities
  // Also check if any b2Body's are out of sync
  world->each<Transform, RigidBody>([&](ECS::Entity* e, ECS::ComponentHandle<Transform> t, ECS::ComponentHandle<RigidBody> r) {

    // Give the RigidBody it's entity
    // @TODO: Implement this in the constructor for RigidBody
    if (r->entity_ == nullptr) {
      r->entity_ = e;
    }

    // Precalculate conversion to radians
    constexpr float convertToRadians = M_PI / 180.f;

    // Check for out of sync
    b2Body* const body = r->body_;
    if (r->isOutOfSync_ && body != nullptr) {
      const b2Vec2 newPos = convertToB2(t->position);
      const float newRot = convertToRadians * t->rotation;
      body->SetTransform(newPos, newRot);
      body->SetAwake(true);
      r->previousPosition_ = newPos;
      r->previousAngle_ = newRot;
      r->isOutOfSync_ = false;
    }
  });

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

  // Tween in between physics steps and destroy any old bodies
  world->each<Transform, RigidBody>([&](ECS::Entity* e, ECS::ComponentHandle<Transform> t, ECS::ComponentHandle<RigidBody> r) {
    smoothState(t, r);

    // Dispose of any old bodies and clear the disposal list
    for (auto* d : r->disposeList_) {
      world_.DestroyBody(d);
    }
    r->disposeList_.clear();
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
  b2Body* const body = rb->body_;
  if (body != nullptr) {

    // Set up variables
    const float oneMinusRatio = 1.0f - fixedTimeStepRatio_;

    // Interpolate based on previous position
    if (body->GetType() != b2_staticBody) {

      // Tween position
      t->position = convertToSF(
        fixedTimeStepRatio_ * body->GetPosition() +
        oneMinusRatio * rb->previousPosition_);

      // Precalculate conversion to degrees
      constexpr float convertToDegrees = 180.f / M_PI;

      // Tween rotation
      t->rotation = convertToDegrees * body->GetAngle();
      // @TODO: This code sets t->rotation to infinity and breaks the game
      // it must be fixed in order to tween rotation
      //t->rotation = convertToDegrees * (body->GetAngle() +
        //oneMinusRatio * rb->previousAngle_);
    }
  }
}

// When the step occurs, reset any smoothing
void
PhysicsSystem::resetSmoothStates(ECS::ComponentHandle<Transform> t, ECS::ComponentHandle<RigidBody> r) {

  // Set up variables
  b2Body* const body = r->body_;

  // Reset any smoothing
  if (body != nullptr && body->GetType() != b2_staticBody) {
    r->previousPosition_ = body->GetPosition();
    r->previousAngle_ = body->GetAngle();
  }
}

// Get this system's physics world
b2World*
PhysicsSystem::getWorld() {
  return &world_;
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
  setGravityVec(sf::Vector2f(gx, gy));
}
void
PhysicsSystem::setGravityVec(const sf::Vector2f& g) {
  world_.SetGravity(convertToB2(g));
}

// Convert to SFML vectors
sf::Vector2f 
PhysicsSystem::convertToSF(const b2Vec2& vec) {
  return sf::Vector2f(vec.x * PhysicsSystem::scale, vec.y * PhysicsSystem::scale);
}

// Convert to Box2D vectors
b2Vec2 
PhysicsSystem::convertToB2(const sf::Vector2f& vec) {
  return b2Vec2(vec.x / PhysicsSystem::scale, vec.y / PhysicsSystem::scale);
}

// Render the physics in debug mode
void
PhysicsSystem::receive(ECS::World* w, const DebugRenderPhysicsEvent& e) {

  // Save the pointer to the window
  physicsDebugDraw_.window = &e.window;

  // Render the physics system
  world_.DrawDebugData();
}

// Add physics entry to the main menu
void
PhysicsSystem::receive(ECS::World* w, const addDebugMenuEntryEvent& e) {
  ImGui::MenuItem("Physics System", NULL, &showPhysicsWindow_);
}

// Add information to debug window
void
PhysicsSystem::receive(ECS::World* w, const addDebugInfoEvent& e) {

  // Add to default window
  ImGui::Begin("Debug");
  ImGui::Text("Physics bodies: %d", world_.GetBodyCount());
  ImGui::End();

  // Make a physics window
  if (showPhysicsWindow_) {
    const auto gravityVec = getGravity();
    float gravity = gravityVec.y / 10.f;
    ImGui::Begin("Physics System", &showPhysicsWindow_);
    ImGui::SliderFloat("Gravity", &gravity, -300.f, 300.f);
    ImGui::End();
    if (gravity * 10.f != gravityVec.y) {
      setGravity(gravityVec.x, gravity * 10.f);
    }
  }
}
