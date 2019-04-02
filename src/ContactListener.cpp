// ContactListener.cpp
// Collision handler for physics with Box2D

#include "ContactListener.h"
#include "RigidBody.h"

// When contact begins
void 
ContactListener::BeginContact(b2Contact* contact) {

  // Check A for fixtures and bodies
  void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
  if (bodyUserData) {
    void* fixtureUserData = contact->GetFixtureA()->GetUserData();
    FixtureType fixtureType = static_cast<FixtureType>((long)fixtureUserData);
    static_cast<RigidBody*>(bodyUserData)->startContact(fixtureType);
  }

  // Check B for fixtures and bodies
  bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
  if (bodyUserData) {
    void* fixtureUserData = contact->GetFixtureB()->GetUserData();
    FixtureType fixtureType = static_cast<FixtureType>((long)fixtureUserData);
    static_cast<RigidBody*>(bodyUserData)->startContact(fixtureType);
  }
}

// When the contact with an entity stops
void 
ContactListener::EndContact(b2Contact* contact) {

  // Check A for fixtures and bodies
  void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
  if (bodyUserData) {
    void* fixtureUserData = contact->GetFixtureA()->GetUserData();
    FixtureType fixtureType = static_cast<FixtureType>((long)fixtureUserData);
    static_cast<RigidBody*>(bodyUserData)->endContact(fixtureType);
  }

  // Check B for fixtures and bodies
  bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
  if (bodyUserData) {
    void* fixtureUserData = contact->GetFixtureB()->GetUserData();
    FixtureType fixtureType = static_cast<FixtureType>((long)fixtureUserData);
    static_cast<RigidBody*>(bodyUserData)->endContact(fixtureType);
  }
}
