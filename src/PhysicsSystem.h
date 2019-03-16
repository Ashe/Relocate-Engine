// PhysicsSystem.h
// System to simulate physics in game

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include <Box2D/Box2D.h>

#include "Game.h"
#include "Transform.h"
#include "RigidBody.h"

#include "PhysicsDebugDraw.h"

class PhysicsSystem : public ECS::EntitySystem, public ECS::EventSubscriber<DebugRenderPhysicsEvent> {
  public:

    // Register a physics system in this world
    static void registerPhysicsSystemFunctions(ECS::World* world);

    // Constructor
    PhysicsSystem();

    // Simulate Physics every update
    virtual void update(ECS::World* world, const sf::Time& dt) override;

    // Subscribe to the DebugDraw method
    virtual void configure(ECS::World* world) override { world->subscribe<DebugRenderPhysicsEvent>(this); }
    virtual void unconfigure(ECS::World* world) override { world->unsubscribeAll(this); }

    // Change the gravity
    void setGravity(float gx, float gy);
    void setGravityMult(float multiplier);
    sf::Vector2f getGravity() const;

  private:

    // Default gravity setting
    const sf::Vector2f defaultGravity_;

    // The box2D world for physics simulation
    b2World world_;

    // Debug rendering system
    PhysicsDebugDraw physicsDebugDraw_;

    // Maximum steps to queue before forcing action
    // A large amount of steps will create more strain,
    // reducing the framerate and creating more steps
    const int maxSteps_ = 5;

    // How long to wait per step
    const float fixedTimeStep_ = 1.0f / 60.0f;

    // Scale between pixels and physics
    const float scale_ = 100.f;

    // A more accurate 'step' for interpolation
    float fixedTimeStepRatio_;

    // An accumilator to make the physics frame independant
    float timeStepAccumilator_;

    int32 velocityIterations_ = 8;
    int32 positionIterations_ = 3;

    void singleStep(float timeStep);
    void smoothState(ECS::ComponentHandle<Transform> t, ECS::ComponentHandle<RigidBody> r);
    void resetSmoothStates(ECS::ComponentHandle<Transform> t, ECS::ComponentHandle<RigidBody> r);
    void ensureRigidBody(ECS::ComponentHandle<Transform> t, ECS::ComponentHandle<RigidBody> r);

    // Conversion functions
    sf::Vector2f convertToSF(const b2Vec2& vec) const;
    b2Vec2 convertToB2(const sf::Vector2f& vec) const; 

    // Render the physics when debug mode is enabled
    virtual void receive(ECS::World* ecsWorld, const DebugRenderPhysicsEvent& ev) override;
};

#endif
