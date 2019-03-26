// RigidBody.cpp
// A component encapsulating Box2D structures b2Body and b2Fixture

#include "RigidBody.h"

// Avoid cyclic dependancies
#include "PhysicsSystem.h"

// Define statics
b2World* RigidBody::worldToSpawnIn_ = nullptr;
b2BodyDef RigidBody::defaultBodyDefinition_ = b2BodyDef();

// Make a box shape
b2Shape* 
RigidBody::BoxShape(float w, float h) {
  b2PolygonShape polygon;
  const auto pos = PhysicsSystem::convertToB2(sf::Vector2f(w, h));
  polygon.SetAsBox(pos.x, pos.y);
  return new b2PolygonShape(polygon);
}

// Make a circle shape
b2Shape* 
RigidBody::CircleShape(float x, float y, float r) {
  b2CircleShape circle;
  const auto pos = PhysicsSystem::convertToB2(sf::Vector2f(x, y));
  circle.m_p.Set(pos.x, pos.y);
  circle.m_radius = r;
  return new b2CircleShape(circle);
}

// Line shape
b2Shape* 
RigidBody::LineShape(float x1, float y1, float x2, float y2) {
  b2EdgeShape line;
  const auto begin = PhysicsSystem::convertToB2(sf::Vector2f(x1, y1));
  const auto end = PhysicsSystem::convertToB2(sf::Vector2f(x2, y2));
  line.Set(begin, end);
  return new b2EdgeShape(line);
}

// Enable use of this component when physics system is enabled
void 
RigidBody::registerFunctions(sol::environment& env, b2World* world) {

  // Debug message
  if (Game::getDebugMode()) {
    printf("Enabling usage of RigidBody components..\n");
  }

  // Set all future rigidbodies to use this world
  // This takes away the responsibility of the programmer to
  // pass the world around
  worldToSpawnIn_ = world;

  // Register additional functions
  env.set_function("BoxShape", &BoxShape);
  env.set_function("CircleShape", &CircleShape);
  env.set_function("LineShape", &LineShape);

  // Register the RigidBody only if there's a 'world'
  if (worldToSpawnIn_ != nullptr) {

    // Register default methods
    Script::registerComponentToEntity<RigidBody>(env, "RigidBody");

    // Create the RigidBody type
    env.new_usertype<RigidBody>("RigidBody",
      // Properties
      "type", sol::property(
        [](const RigidBody& self) {return self.body_->GetType();},
        [](RigidBody& self, const b2BodyType& type) { self.body_->SetType(type);}),
      "gravity", sol::property(
        [](const RigidBody& self) {return self.body_->GetGravityScale();},
        [](RigidBody& self, float g) {self.body_->SetGravityScale(g);}),
      "isFixedRotation", sol::property(
        [](const RigidBody& self) {return self.body_->IsFixedRotation();},
        [](RigidBody& self, bool isFixed) { self.body_->SetFixedRotation(isFixed);}),
      // Basic functions
      "instantiate", &RigidBody::instantiateBody,
      "addFixture", &RigidBody::addFixture,
      "getLocation", [](const RigidBody& self){return PhysicsSystem::convertToSF(self.body_->GetWorldCenter());},
      "warpTo", sol::overload(&RigidBody::warpTo, &RigidBody::warpToVec),
      "setLinearVelocity", sol::overload(&RigidBody::setLinearVelocity, &RigidBody::setLinearVelocityVec),
      // Forces
      "applyForce", sol::overload(&RigidBody::applyForce, &RigidBody::applyForceVec),
      "applyForceToCentre", sol::overload(&RigidBody::applyForceToCentre, &RigidBody::applyForceToCentreVec),
      "applyForceRel", sol::overload(&RigidBody::applyForceRel, &RigidBody::applyForceRelVec),
      "applyImpulse", sol::overload(&RigidBody::applyImpulse, &RigidBody::applyImpulseVec),
      "applyImpulseToCentre", sol::overload(&RigidBody::applyImpulseToCentre, &RigidBody::applyImpulseToCentreVec),
      "applyImpulseRel", sol::overload(&RigidBody::applyImpulseRel, &RigidBody::applyImpulseRelVec)
    );
  }

  // Define mouse joint creation
  env.set_function("createMouseJoint", 
      [world](const b2MouseJointDef& def){return (b2MouseJoint*) world->CreateJoint(&def);});
  env.new_usertype<b2MouseJointDef>("MouseJointDef",
    sol::constructors<b2MouseJointDef()>(),
    "dampingRatio", &b2MouseJointDef::dampingRatio,
    "frequency", &b2MouseJointDef::frequencyHz,
    "maxForce", &b2MouseJointDef::maxForce,
    "foo", &b2MouseJointDef::bodyA,
    "setBodyA", [](b2MouseJointDef& self, RigidBody& body){self.bodyA = body.body_;},
    "setBodyB", [](b2MouseJointDef& self, RigidBody& body){self.bodyB = body.body_;},
    "target", sol::property(
      [](const b2MouseJointDef& self) {return PhysicsSystem::convertToSF(self.target);},
      [](b2MouseJointDef& self, const sf::Vector2f& target) {self.target = PhysicsSystem::convertToB2(target);})
  );
  env.new_usertype<b2MouseJoint>("MouseJoint",
   "destroy", [world](b2MouseJoint& self){world->DestroyJoint(&self);},
   "getAnchorA", &b2MouseJoint::GetAnchorA,
   "getAnchorB", &b2MouseJoint::GetAnchorB,
   "frequency", sol::property(&b2MouseJoint::GetFrequency, &b2MouseJoint::SetFrequency),
   "maxForce", sol::property(&b2MouseJoint::GetMaxForce, &b2MouseJoint::SetMaxForce),
   "dampingRatio", sol::property(&b2MouseJoint::GetDampingRatio, &b2MouseJoint::SetDampingRatio),
   "target", sol::property(
     [](const b2MouseJoint& self) {return PhysicsSystem::convertToSF(self.GetTarget());},
     [](b2MouseJoint& self, const sf::Vector2f& target) {self.SetTarget(PhysicsSystem::convertToB2(target));})
  );
}

// Register functions that don't need a physics world
void
RigidBody::registerNonDependantFunctions() {
  // Create the BodyDef type
  Game::lua.new_usertype<b2BodyDef>("BodyDef",
    sol::constructors<b2BodyDef()>(),
    "type", &b2BodyDef::type,
    "isFixedRotation", &b2BodyDef::fixedRotation
  );

  // Different body types
  Game::lua.set("Physics_DynamicBody", b2_dynamicBody);
  Game::lua.set("Physics_KinematicBody", b2_kinematicBody);
  Game::lua.set("Physics_StaticBody", b2_staticBody);

  // Create the FixtureDef type
  Game::lua.new_usertype<b2FixtureDef>("FixtureDef",
    sol::constructors<b2FixtureDef()>(),
    "shape", &b2FixtureDef::shape,
    "density", sol::property(
      [](const b2FixtureDef& self) {return self.density * PhysicsSystem::scale;},
      [](b2FixtureDef& self, float d) {self.density = d / PhysicsSystem::scale;}),
    "friction", sol::property(
      [](const b2FixtureDef& self) {return self.friction * PhysicsSystem::scale;},
      [](b2FixtureDef& self, float f) {self.friction = f / PhysicsSystem::scale;}),
    "restitution", sol::property(
      [](const b2FixtureDef& self) {return self.restitution * PhysicsSystem::scale; },
      [](b2FixtureDef& self, float r) {self.restitution = r / PhysicsSystem::scale;})
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

// Warp somewhere instantaneously
void 
RigidBody::warpTo(float x, float y) {
  warpToVec(sf::Vector2f(x,y));
}
void 
RigidBody::warpToVec(const sf::Vector2f& dest) {
  body_->SetTransform(PhysicsSystem::convertToB2(dest), body_->GetAngle());
  body_->SetLinearVelocity(b2Vec2(0,0));
}

// Set linear velocity to RigidBody
void
RigidBody::setLinearVelocity(float i, float j) {
  setLinearVelocityVec(sf::Vector2f(i, j));
}
void
RigidBody::setLinearVelocityVec(const sf::Vector2f& vel) {
  body_->SetLinearVelocity(PhysicsSystem::convertToB2(vel));
}

// Apply a force to the RigidBody
void 
RigidBody::applyForceToCentre(float i, float j) {
  applyForceToCentreVec(sf::Vector2f(i,j));
}
void 
RigidBody::applyForceToCentreVec(const sf::Vector2f& force) {
  body_->ApplyForceToCenter(PhysicsSystem::convertToB2(force), true);
}

// Apply a force relative to the body's centre
void 
RigidBody::applyForceRel(float i, float j, float x, float y) {
  applyForceRelVec(sf::Vector2f(i, j), sf::Vector2f(x, y));
}
void 
RigidBody::applyForceRelVec(const sf::Vector2f& force, const sf::Vector2f& relPos) {
  body_->ApplyForce(PhysicsSystem::convertToB2(force), body_->GetWorldPoint(PhysicsSystem::convertToB2(relPos)), true);
}

// Apply a force to this body from somewhere
void 
RigidBody::applyForce(float i, float j, float x, float y) {
  applyForceVec(sf::Vector2f(i, j), sf::Vector2f(x, y));
}
void 
RigidBody::applyForceVec(const sf::Vector2f& force, const sf::Vector2f& location) {
  body_->ApplyForce(PhysicsSystem::convertToB2(force), PhysicsSystem::convertToB2(location), true);
}

// Apply an impulse to the RigidBody
void
RigidBody::applyImpulseToCentre(float i, float j) {
  applyImpulseToCentreVec(sf::Vector2f(i, j));
}
void 
RigidBody::applyImpulseToCentreVec(const sf::Vector2f& impulse) {
  body_->ApplyLinearImpulse(PhysicsSystem::convertToB2(impulse), body_->GetWorldCenter(), true);
}

// Apply an impulse relative to the body's centre
void
RigidBody::applyImpulseRel(float i, float j, float x, float y) {
  applyImpulseRelVec(sf::Vector2f(i, j), sf::Vector2f(x, y));
}
void
RigidBody::applyImpulseRelVec(const sf::Vector2f& impulse, const sf::Vector2f& relPos) {
  body_->ApplyLinearImpulse(PhysicsSystem::convertToB2(impulse), body_->GetWorldPoint(PhysicsSystem::convertToB2(relPos)), true);
}

// Apply an impulse to this body from somewhere
void
RigidBody::applyImpulse(float i, float j, float x, float y) {
  applyImpulseVec(sf::Vector2f(i, j), sf::Vector2f(x, y));
}
void
RigidBody::applyImpulseVec(const sf::Vector2f& impulse, const sf::Vector2f& location) {
  body_->ApplyLinearImpulse(PhysicsSystem::convertToB2(impulse), PhysicsSystem::convertToB2(location), true);
}
