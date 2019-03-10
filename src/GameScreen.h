// GameScreen.h
// The screen where the game takes place

#ifndef GAMESCREEN_H
#define GAMESCREEN_H

#include "Game.h"
#include "Screen.h"
#include "PhysicsSystem.h"

class GameScreen : public Screen {
  public:
    GameScreen();
    ~GameScreen();

    // Override Screen functions
    void showScreen() override;
    void hideScreen() override;
    void update(const sf::Time& dt) override;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;
    void quit() override;

  private:

    // This screen contains it's own ECS system
    ECS::World* world;
};

class DebugSystem : public ECS::EntitySystem {
  public:
    virtual void update(ECS::World* world, const sf::Time& dt) override {
      world->each<Transform>([&](ECS::Entity* ent, ECS::ComponentHandle<Transform> t) {
        //printf("%f\n", t->position.y);
      });
    }
};

#endif
