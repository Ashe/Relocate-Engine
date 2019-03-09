// GameScreen.h
// The screen where the game takes place

#ifndef GAMESCREEN_H
#define GAMESCREEN_H

#include "Game.h"
#include "Screen.h"

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

struct Position {
  Position(float x, float y) : x(x), y(y) {}
  Position() : x(0.f), y(0.f) {}

  float x;
  float y;
};

struct Rotation {
  Rotation(float angle) : angle(angle) {}
  Rotation() : angle(0) {}
  float angle;
};

class GravitySystem : public ECS::EntitySystem {
  public:
    GravitySystem(float amount) {
      gravityAmount = amount;
    }

    virtual ~GravitySystem() {} 
    virtual void tick(ECS::World* world, sf::Time dt) override {
      world->each<Position>([&](ECS::Entity* ent, ECS::ComponentHandle<Position> position) {
        position->y += gravityAmount * dt.asSeconds();
      });
    }

  private:
    float gravityAmount;
};

class DebugSystem : public ECS::EntitySystem {
  public:
    DebugSystem() {}

    virtual ~DebugSystem() {} 
    virtual void tick(ECS::World* world, sf::Time dt) override {
      world->each<Position>([&](ECS::Entity* ent, ECS::ComponentHandle<Position> position) {
        printf("%f\n", position ->y);
      });
    }

  private:
    float gravityAmount;
};

#endif
