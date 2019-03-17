// RigidBody.h
// A component encapsulating Box2D structures b2Body and b2Fixture

#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "Game.h"
#include "Scripting.h"
#include <Box2D/Box2D.h>
#include <vector>

class RigidBody {
  public:

    // Friend of the physics system
    friend class PhysicsSystem;

    // Make different shapes
    static b2Shape* BoxShape(float w, float h);
    static b2Shape* CircleShape(float x, float y, float r);
    static b2Shape* LineShape(float x1, float y1, float x2, float y2);

    // Make this component scriptable
    static void registerFunctions(b2World* world);

    // Constructors
    RigidBody();
    ~RigidBody();

    // Create the b2Body out of a b2BodyDef
    void instantiateBody(const b2BodyDef& def);

    // Add a fixture to this body
    void addFixture(const b2FixtureDef& def);

    // We have static operators so this operator must be defined
    void operator= (const RigidBody& other) { 
       body_ = other.body_;
       previousPosition_ = other.previousPosition_;
       previousAngle_ = other.previousAngle_;
       isOutOfSync_ = other.isOutOfSync_;
    }

  private:

    // Default body definition to use
    static b2BodyDef defaultBodyDefinition_;

    // The world of this object, not to be confused with the ECS world
    static b2World* worldToSpawnIn_;
    b2World* const physics_;

    // The encapsulated body of this object
    b2Body* body_;

    // Manipulated by physics system
    b2Vec2 previousPosition_;
    float previousAngle_;

    // Marks whether this b2Body's position is out of sync with transform
    bool isOutOfSync_;

    // List of b2Bodys to destroy
    std::vector<b2Body*> disposeList_;
};

#endif
