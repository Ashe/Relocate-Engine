// RigidBody.h
// A component encapsulating Box2D structure

#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "Game.h"
#include "Scripting.h"
#include <Box2D/Box2D.h>
#include <vector>

class RigidBody {
  public:

    // Make a RigidBody and assign to an entity
    static RigidBody& assign(ECS::Entity* e);

    // Make different shapes
    static b2Shape* BoxShape(float w, float h);
    static b2Shape* CircleShape(float x, float y, float r);
    static b2Shape* LineShape(float x1, float y1, float x2, float y2);

    // Make this component scriptable
    static void registerFunctions();

    // Constructors
    RigidBody();
    ~RigidBody();

    // Manipulated by physics system
    b2Vec2 previousPosition_;
    float previousAngle_;


    // Only change the body if there is none
    void setBody(b2Body* body);

    // Retrieve the body, whatever it is
    b2Body* const getBody();

    // Only set the bodydef if there's no body yet
    void setBodyDef(const b2BodyDef& bodyDef);
    const b2BodyDef& getBodyDef();

    // Only set fixturedefs if there's no body yet
    void addFixtureDef(const b2FixtureDef& fixtureDef);
    const std::vector<b2FixtureDef>& getFixtureDefs();

  private:
    b2Body* body_;
    b2BodyDef bodyDef_;
    std::vector<b2FixtureDef> fixtureDefs_;
};

#endif
