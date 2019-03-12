// Scene.cpp
// Logic more oriented to the game goes here

#include "Scene.h"

// Constructor
Scene::Scene() {

  // Create the ECS world
  world_ = ECS::World::createWorld();
  world_->registerSystem(new PhysicsSystem());

  // Do some physics
  const sf::Vector2u size = Game::getWindow()->getSize();

  // Create the ground
  ECS::Entity* ground = world_->create();
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
  ECS::Entity* box = world_->create();
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
Scene::~Scene() {
  world_->destroyWorld();
}

// Called when the scene is started
void
Scene::begin() {
}

// When the screen is shown
void
Scene::showScene() {
}

// When the screen is hidden
void
Scene::hideScene() {
}

// Update the game every frame
void
Scene::update(const sf::Time& dt) {

  // Update the ECS
  world_->update(dt);

  // Do any debug-only logic
  if (Game::getDebugMode()) {
    //@TODO: Debug update functions here
  }
}

// Render the game every frame
void
Scene::render(sf::RenderWindow& window) {

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
    world_->emit<DebugRenderPhysicsEvent>({});
  }
}

// Handle keypresses
void
Scene::handleEvent(const sf::Event& event) {
  if (event.type == sf::Event::KeyPressed) {

    // Toggle debug mode on F1
    if (event.key.code == sf::Keyboard::F1) {
      Game::setDebugMode(!Game::getDebugMode());
    }
  }
}

// When the game is quit
void
Scene::quit() {
  Game::terminate();
}

