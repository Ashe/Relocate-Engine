// RigidBody.cpp
// A component encapsulating Box2D structures b2Body and b2Fixture

#include "RigidBody.h"

// Define statics
b2World* RigidBody::worldToSpawnIn_ = nullptr;
b2BodyDef RigidBody::defaultBodyDefinition_ = b2BodyDef();

// Make a box shape
b2Shape* 
RigidBody::BoxShape(float w, float h) {
  b2PolygonShape polygon;
  polygon.SetAsBox(w, h);
  return new b2PolygonShape(polygon);
}

// Make a circle shape
b2Shape* 
RigidBody::CircleShape(float x, float y, float r) {
  b2CircleShape circle;
  circle.m_p.Set(x, y);
  circle.m_radius = r;
  return new b2CircleShape(circle);
}

// Line shape
b2Shape* 
RigidBody::LineShape(float x1, float y1, float x2, float y2) {
  b2EdgeShape line;
  line.Set(b2Vec2(x1, y1), b2Vec2(x2, y2));
  return new b2EdgeShape(line);
}

// Enable use of this component when physics system is enabled
void 
RigidBody::registerFunctions(b2World* world) {

  // Debug message
  if (Game::getDebugMode()) {
    printf("Enabling usage of RigidBody components..\n");
  }

  // Set all future rigidbodies to use this world
  // This takes away the responsibility of the programmer to
  // pass the world around
  worldToSpawnIn_ = world;

  // Register default methods
  Script::registerComponentDefaults<RigidBody>("RigidBody");

  // Register additional functions
  Game::lua.set_function("BoxShape", &BoxShape);
  Game::lua.set_function("CircleShape", &CircleShape);
  Game::lua.set_function("LineShape", &LineShape);

  // Create the RigidBody type
  Game::lua.new_usertype<RigidBody>("RigidBody",
    sol::constructors<RigidBody()>(),
    "instantiate", &RigidBody::instantiateBody,
    "addFixture", &RigidBody::addFixture
  );

  // Different body types
  Game::lua.set("Physics_DynamicBody", b2_dynamicBody);
  Game::lua.set("Physics_KinematicBody", b2_kinematicBody);
  Game::lua.set("Physics_StaticBody", b2_staticBody);

  // Create the BodyDef type
  Game::lua.new_usertype<b2BodyDef>("BodyDef",
    sol::constructors<b2BodyDef()>(),
    "type", &b2BodyDef::type
  );

  // Create the FixtureDef type
  Game::lua.new_usertype<b2FixtureDef>("FixtureDef",
    sol::constructors<b2FixtureDef()>(),
    "shape", &b2FixtureDef::shape,
    "density", &b2FixtureDef::density,
    "friction", &b2FixtureDef::friction,
    "restitution", &b2FixtureDef::restitution
  );
}

// Constructor
RigidBody::RigidBody()
  : physics_(worldToSpawnIn_)
  , body_(physics_->CreateBody(&defaultBodyDefinition_))
  , previousPosition_(b2Vec2(0.0f, 0.0f))
  , previousAngle_(0.0f) 
  , isOutOfSync_(true) {}

// Destructor
RigidBody::~RigidBody() {}

// Recreate the b2Body out of a b2BodyDef
void
RigidBody::instantiateBody(const b2BodyDef& def) {

  // If the body already exists, destroy it
  if (body_ != nullptr) {
   disposeList_.push_back(body_);
   body_ = nullptr;
  }

  // Create a new body out of the new definitions
  body_ = physics_->CreateBody(&def);

  // Mark this RididBody as out of sync with it's transform
  isOutOfSync_ = true;
}

// Add a fixture to this RigidBody
void
RigidBody::addFixture(const b2FixtureDef& def) {
  body_->CreateFixture(&def);
}
