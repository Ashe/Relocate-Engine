// ContactListener.h
// Collision handler for physics with Box2D

#ifndef CONTACTLISTENER_H
#define CONTACTLISTENER_H

#include <Box2D/Box2D.h>

// Forward declaration
class RigidBody;

// Fixture classification
enum FixtureType
{ Unknown
, GroundSensor
};

// Class which resolves collisions
class ContactListener : public b2ContactListener {
  void BeginContact(b2Contact* contact);
  void EndContact(b2Contact* contact);
};

#endif
