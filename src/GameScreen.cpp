// GameScreen.cpp
// The screen where the game takes place

#include "GameScreen.h"

// Constructor
GameScreen::GameScreen() {

  // Create the ECS world
  world = ECS::World::createWorld();
  world->registerSystem(new GravitySystem(-9.8f));
  world->registerSystem(new DebugSystem());

  ECS::Entity* ent = world->create();
  ent->assign<Position>(0.f, 0.f); // assign() takes arguments and passes them to the constructor
  ent->assign<Rotation>(35.f);
}

// Destructor
GameScreen::~GameScreen() {
  world->destroyWorld();
}

// When the screen is shown
void
GameScreen::showScreen() {
  printf("Screen changed to: Game\n");
}

// When the screen is hidden
void
GameScreen::hideScreen() {
  printf("Screen changed from: Game\n");
}

// Update the game every frame
void
GameScreen::update(const sf::Time& dt) {
  world->tick(dt);
}

// Render the game every frame
void
GameScreen::render(sf::RenderWindow& window) {

  // Render a simple circle for testing
  float rad = 100.0f;
  sf::CircleShape shape(rad);
  const sf::Vector2u size = Game::getWindow()->getSize();
  auto pos = sf::Vector2f((size.x * 0.5f) - rad, (size.y * 0.5f) - rad);
  shape.setPosition(pos);

  shape.setFillColor(sf::Color::Green);
  window.draw(shape);
}

// Handle keypresses
void
GameScreen::handleEvent(const sf::Event& event) {
}

// When the game is quit
void
GameScreen::quit() {
  printf("Quitting screen: Game\n");
}
