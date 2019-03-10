// GameScreen.cpp
// The screen where the game takes place

#include "GameScreen.h"

// Constructor
GameScreen::GameScreen() {

  // Create the ECS world
  world = ECS::World::createWorld();
  world->registerSystem(new PhysicsSystem());
  world->registerSystem(new DebugSystem());

  // Do some physics
  const sf::Vector2u size = Game::getWindow()->getSize();

  // Create the ground
  ECS::Entity* ground = world->create();
  auto groundPos = ground->assign<Transform>();
  auto groundBody = ground->assign<RigidBody>();
  b2BodyDef groundBodyDef;
  b2PolygonShape groundBox;
  b2FixtureDef groundFixture;
  
  groundPos->position = sf::Vector2i(size.x * 0.5f, size.y * 0.75f);
  groundBody->setBodyDef(groundBodyDef);
  groundBox.SetAsBox(size.x, 10.0f);
  groundFixture.shape = new b2PolygonShape(groundBox);
  groundBody->addFixtureDef(groundFixture);
  
  // Create the box
  ECS::Entity* box = world->create();
  auto boxPos = box->assign<Transform>();
  auto boxBody = box->assign<RigidBody>();
  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;

  boxPos->position = sf::Vector2i(size.x * 0.5f, 0.0f);
  boxBody->setBodyDef(bodyDef);
  b2PolygonShape dynamicBox;
  b2FixtureDef fixtureDef;
  dynamicBox.SetAsBox(2.0f, 2.0f);
  fixtureDef.shape = new b2PolygonShape(dynamicBox);
  fixtureDef.density = 20.0f;
  fixtureDef.friction = 0.3f;
  boxBody->addFixtureDef(fixtureDef);
}

// Destructor
GameScreen::~GameScreen() {
  world->destroyWorld();
}

// When the screen is shown
void
GameScreen::showScreen() {
}

// When the screen is hidden
void
GameScreen::hideScreen() {
}

// Update the game every frame
void
GameScreen::update(const sf::Time& dt) {
  world->update(dt);

  // Do any debug-only logic
  if (Game::getDebugMode()) {
    //@TODO: Debug update functions here
  }
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

  // Do any debug-only rendering
  if (Game::getDebugMode()) {
    world->emit<DebugRenderPhysicsEvent>({});
  }
}

// Handle keypresses
void
GameScreen::handleEvent(const sf::Event& event) {
  if (event.type == sf::Event::KeyPressed) {

    // Toggle debug mode on F1
    if (event.key.code == sf::Keyboard::F1) {
      Game::setDebugMode(!Game::getDebugMode());
    }
  }
}

// When the game is quit
void
GameScreen::quit() {
  Game::terminate();
}
