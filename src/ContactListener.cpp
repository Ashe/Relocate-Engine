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
  float mult = 0.f;
  if (fixtureAType != FixtureType::GroundSensor && fixtureBType != GroundSensor) {

    // Get the world manifold
    b2WorldManifold worldManifold;
    contact->GetWorldManifold( &worldManifold );

    // Get masses for bodies
    const float massA = bodyA->GetMass();
    const float massB = bodyB->GetMass();

    // Calculate momentum for body A
    b2Vec2 vel1 = bodyA->GetLinearVelocityFromWorldPoint( worldManifold.points[0] );
    vel1.x *= massA;
    vel1.y *= massA;

    // Calculate momentum for body B
    b2Vec2 vel2 = bodyB->GetLinearVelocityFromWorldPoint( worldManifold.points[0] );
    vel2.x *= massB;
    vel2.y *= massB;

    // Calculate the mass based on the differences in momentum
    impact = abs((vel1 - vel2).Length());
  }

  // Check A for fixtures and bodies if it has userdata to pass to
  if (bodyA->GetUserData() != nullptr) {
    RigidBody* other = static_cast<RigidBody*>(bodyB->GetUserData());
    static_cast<RigidBody*>(bodyA->GetUserData())->startContact(fixtureAType, other, impact);
  }

  // Check B for fixtures and bodies if it has userdata to pass to
  if (bodyB->GetUserData() != nullptr) {
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
