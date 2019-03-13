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
    Scene();
    ~Scene();

    void begin();
    void update(const sf::Time& dt);
    void render(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event);
    void showScene();
    void hideScene();
    void quit();

  private:

    // The ECS for this scene
    ECS::World* world_;
};

#endif
