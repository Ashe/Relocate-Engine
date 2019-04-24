// PhysicsSystem.h
// System to simulate physics in game

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#define _USE_MATH_DEFINES
#include <math.h>

#include <Box2D/Box2D.h>

#include "Game.h"
#include "Transform.h"
#include "RigidBody.h"

#include "PhysicsDebugDraw.h"

class PhysicsSystem 
: public ECS::EntitySystem
, public ECS::EventSubscriber<DebugRenderPhysicsEvent>
, public ECS::EventSubscriber<addDebugInfoEvent>
, public ECS::EventSubscriber<addDebugMenuEntryEvent> {
  public:

    // Friend class
    friend class PhysicsDebugDraw;

    // Register a Physics System in this world
    static void registerPhysicsSystem(sol::environment& env, ECS::World* world);

    // Conversion functions
    static sf::Vector2f convertToSF(const b2Vec2& vec);
    static b2Vec2 convertToB2(const sf::Vector2f& vec);

    // Scale between pixels and physics
    static const float scale;

    // Constructors
    PhysicsSystem();
    ~PhysicsSystem();

    // Simulate Physics every update
    virtual void update(ECS::World* world, const sf::Time& dt) override;

    // Subscribe to the DebugDraw method
    virtual void configure(ECS::World* world) override { 
      world->subscribe<DebugRenderPhysicsEvent>(this); 
      world->subscribe<addDebugMenuEntryEvent>(this); 
      world->subscribe<addDebugInfoEvent>(this); 
    }
    virtual void unconfigure(ECS::World* world) override { world->unsubscribeAll(this); }

    // Get this physics system's world
    b2World* getWorld();

    // Change the gravity
    void setGravityMult(float multiplier);
    sf::Vector2f getGravity() const;
    void setGravity(float gx, float gy);
    void setGravityVec(const sf::Vector2f& g);

  private:

    // ContactListener
    ContactListener contactListener_;

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

    // A more accurate 'step' for interpolation
    float fixedTimeStepRatio_;

    // An accumilator to make the physics frame independant
    float timeStepAccumilator_;

    int32 velocityIterations_ = 8;
    int32 positionIterations_ = 3;

    // Imgui flags
    static bool showPhysicsWindow_;
    static bool showRigidBodies_;

    // Step through physics once
    void singleStep(float timeStep);

    // Interpolate between physics steps
    void smoothState(ECS::ComponentHandle<Transform> t, ECS::ComponentHandle<RigidBody> r);

    // Reset interpolation to the actual physics locations
    void resetSmoothStates(ECS::ComponentHandle<RigidBody> r);

    // Render the physics when debug mode is enabled
    virtual void receive(ECS::World* ecsWorld, const DebugRenderPhysicsEvent& ev) override;

    // Add physics menu entry to menu window
    virtual void receive(ECS::World* ecsWorld, const addDebugMenuEntryEvent& ev) override;

    // Add the location of the player to debug window
    virtual void receive(ECS::World* ecsWorld, const addDebugInfoEvent& ev) override;
};

#endif
