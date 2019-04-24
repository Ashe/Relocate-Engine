// ContactListener.cpp
// Collision handler for physics with Box2D

#include "ContactListener.h"
#include "RigidBody.h"

// When contact begins
void 
ContactListener::BeginContact(b2Contact* contact) {

  // Get bodies
  b2Body* const bodyA = contact->GetFixtureA()->GetBody();
  b2Body* const bodyB = contact->GetFixtureB()->GetBody();

  // Get fixtures
  FixtureType fixtureAType = static_cast<FixtureType>((long)contact->GetFixtureA()->GetUserData());
  FixtureType fixtureBType = static_cast<FixtureType>((long)contact->GetFixtureB()->GetUserData());

  // Get the velocity of the collision
  double impact = 0.0;
  if (fixtureAType != FixtureType::GroundSensor && fixtureBType != GroundSensor) {

    // Get the world manifold
    b2WorldManifold worldManifold;
    contact->GetWorldManifold( &worldManifold );

    // Calculate impact force
    const b2Vec2 vel1 = bodyA->GetLinearVelocityFromWorldPoint( worldManifold.points[0] );
    const b2Vec2 vel2 = bodyB->GetLinearVelocityFromWorldPoint( worldManifold.points[0] );
    impact = abs((vel1 - vel2).Length());
  }

  // Check A for fixtures and bodies if it has userdata to pass to
  if (bodyA->GetUserData() != nullptr) {
    RigidBody* other = static_cast<RigidBody*>(bodyB->GetUserData());
    static_cast<RigidBody*>(bodyA->GetUserData())->startContact(fixtureAType, other, impact);
  }

  // Check B for fixtures and bodies if it has userdata to pass to
  if (bodyB->GetUserData() != nullptr) {
    void* fixtureUserData = contact->GetFixtureB()->GetUserData();
    RigidBody* other = static_cast<RigidBody*>(bodyA->GetUserData());
    static_cast<RigidBody*>(bodyB->GetUserData())->startContact(fixtureBType, other, impact);
  }
}

// When the contact with an entity stops
void 
ContactListener::EndContact(b2Contact* contact) {

  // Get bodies
  b2Body* const bodyA = contact->GetFixtureA()->GetBody();
  b2Body* const bodyB = contact->GetFixtureB()->GetBody();

  // Get fixtures
  FixtureType fixtureAType = static_cast<FixtureType>((long)contact->GetFixtureA()->GetUserData());
  FixtureType fixtureBType = static_cast<FixtureType>((long)contact->GetFixtureB()->GetUserData());

  // Check A for fixtures and bodies if it has userdata to pass to
  if (bodyA->GetUserData() != nullptr) {
    RigidBody* other = static_cast<RigidBody*>(bodyB->GetUserData());
    static_cast<RigidBody*>(bodyA->GetUserData())->endContact(fixtureAType, other);
  }

  // Check B for fixtures and bodies if it has userdata to pass to
  if (bodyB->GetUserData() != nullptr) {
    RigidBody* other = static_cast<RigidBody*>(bodyA->GetUserData());
    static_cast<RigidBody*>(bodyB->GetUserData())->endContact(fixtureBType, other);
  }
}
