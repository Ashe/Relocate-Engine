// Scene.h
// Game oriented logic goes here

#ifndef GAMESCREEN_H
#define GAMESCREEN_H

#include <string>
#include <iostream>

#include "Game.h"
#include "PhysicsSystem.h"

class Scene {
  public:
    Scene(const std::string& script);
    ~Scene();

    void begin();
    void registerFunctions();
    void update(const sf::Time& dt);
    void render(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event);
    void showScene();
    void hideScene();
    void quit();

  private:

    // If the scene has begun
    bool hasBegun_;

    // The ECS for this scene
    ECS::World* world_;

    // Script file
    const std::string& script_;
};

#endif
