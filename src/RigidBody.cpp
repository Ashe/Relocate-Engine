// RigidBody.cpp
// A component encapsulating Box2D structure

#include "RigidBody.h"


// Make a RigidBody and assign to an entity
RigidBody& 
RigidBody::assign(ECS::Entity* e) {
  return (e->assign<RigidBody>()).get();
}

// Make a box shape
b2Shape* 
RigidBody::BoxShape(float w, float h) {
  b2PolygonShape polygon;
  polygon.SetAsBox(w, h);
  return new b2PolygonShape(polygon);
}

// Make this component scriptable
void 
RigidBody::registerFunctions() {

  // Register assign method
  Game::lua.set_function("assignRigidBody", &assign);
  Game::lua.set_function("BoxShape", &BoxShape);

  // Create the RigidBody type
  Game::lua.new_usertype<RigidBody>("RigidBody",
    sol::constructors<RigidBody()>(),
    "bodyDef", sol::property(&RigidBody::getBodyDef, &RigidBody::setBodyDef),
    "addFixtureDef", &RigidBody::addFixtureDef
  );

  // Different body types
  Game::lua.set("Physics_DynamicBody", b2_dynamicBody);
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
    "friction", &b2FixtureDef::friction
  );
}

// Constructor
RigidBody::RigidBody()
  : previousPosition_(b2Vec2(0.0f, 0.0f))
  , previousAngle_(0.0f)
  , body_(nullptr) {}

RigidBody::~RigidBody() {
  // Delete all fixture shapes
  for (auto& fixture : fixtureDefs_) {
    if (fixture.shape != nullptr) {
      delete fixture.shape;
      fixture.shape = nullptr;
    }
  }
}

// Only change the body if there is none
void 
RigidBody::setBody(b2Body* body) {
  if (body_ == nullptr) {
    body_ = body;
  }
}

// Retrieve the body, whatever it is
b2Body* const
RigidBody::getBody() {
  return body_;
}

// Only set the bodydef if there's no body yet
void 
RigidBody::setBodyDef(const b2BodyDef& bodyDef) {
  if (body_ == nullptr) {
    bodyDef_ = b2BodyDef(bodyDef);
  }
}
const b2BodyDef& 
RigidBody::getBodyDef() {
  return bodyDef_;
}

// Only set fixturedefs if there's no body yet
void 
RigidBody::addFixtureDef(const b2FixtureDef& fixtureDef) {
  if (body_ == nullptr) {
    fixtureDefs_.push_back(fixtureDef);
  }
}

// Get fixturedefs
const std::vector<b2FixtureDef>& 
RigidBody::getFixtureDefs() {
  return fixtureDefs_;
}
