// Scene.h
// Game oriented logic goes here

#ifndef GAMESCREEN_H
#define GAMESCREEN_H

#include <string>
#include <memory>
#include <queue>

#include "Game.h"
#include "Scripting.h"
#include "PhysicsSystem.h"

// Represents it's own world of objects
class Scene {
  public:

    // Register scene functionality to Lua
    static void registerSceneType();

    // Constructors
    Scene();

    // Copy constructor
    Scene(const Scene& other);

    // Destructor
    ~Scene();

    // Handle standard Game functions
    void begin();
    void registerFunctions();
    void update(const sf::Time& dt);
    void render(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event);
    void showScene();
    void hideScene();
    void quit();

    // Add a menu entry to the debug menu
    void addDebugMenuEntries();

    // Add functionality to the default window
    void addDebugInfoToDefault();

  private:

    // If the scene has begun
    bool hasBegun_;

    // The ECS for this scene
    ECS::World* world_;

    // The environment for this scene
    sol::environment lua_;

    // Functions for this scene
    sol::protected_function onBegin_;
    sol::protected_function onShow_;
    sol::protected_function onHide_;
    sol::protected_function onUpdate_;
    sol::protected_function onWindowEvent_;
    sol::protected_function onQuit_;

    // The queue to render
    std::priority_queue<const Renderable*, std::vector<const Renderable*>, cmpRenderable> drawList_;
};

#endif
