// RigidBody.h
// A component encapsulating Box2D structures b2Body and b2Fixture

#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "Game.h"
#include "Scripting.h"
#include <Box2D/Box2D.h>
#include <vector>

// Encapsulate physics interactions
class RigidBody {
  public:

    // Friend of the physics system
    friend class PhysicsSystem;

    // Make different shapes
    static b2Shape* BoxShape(float w, float h);
    static b2Shape* CircleShape(float x, float y, float r);
    static b2Shape* LineShape(float x1, float y1, float x2, float y2);

    // Make this component scriptable
    static void registerFunctions(sol::environment& env, b2World* world);
    static void registerNonDependantFunctions();

    // Constructors
    RigidBody();
    ~RigidBody();

    // Create the b2Body out of a b2BodyDef
    void instantiateBody(const b2BodyDef& def);

    // Add a fixture to this body
    void addFixture(const b2FixtureDef& def);

    // Warp this entity to a location instantaneously
    void warpTo(float x, float y);
    void warpToVec(const sf::Vector2f& dest);

    // Set linear velocity
    void setLinearVelocity(float i, float j);
    void setLinearVelocityVec(const sf::Vector2f& vel);

    // Get linear velocity
    sf::Vector2f getLinearVelocity() const;

    // Apply a force to this body
    void applyForceToCentre(float i, float j);
    void applyForceToCentreVec(const sf::Vector2f& force);

    // Apply a force relative to this body's position
    void applyForceRel(float i, float j, float x, float y);
    void applyForceRelVec(const sf::Vector2f& force, const sf::Vector2f& relPos);

    // Apply a force to this body from somewhere
    void applyForce(float i, float j, float x, float y);
    void applyForceVec(const sf::Vector2f& force, const sf::Vector2f& location);

    // Apply an impulse to this body
    void applyImpulseToCentre(float i, float j);
    void applyImpulseToCentreVec(const sf::Vector2f& impulse);

    // Apply an impulse relative to this body's position
    void applyImpulseRel(float i, float j, float x, float y);
    void applyImpulseRelVec(const sf::Vector2f& impulse, const sf::Vector2f& relPos);

    // Apply an impulse to this body from somewhere
    void applyImpulse(float i, float j, float x, float y);
    void applyImpulseVec(const sf::Vector2f& impulse, const sf::Vector2f& location);

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
