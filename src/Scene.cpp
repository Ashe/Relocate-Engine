// Scene.cpp
// Logic more oriented to the game goes here

#include "Scene.h"

// Constructor
Scene::Scene(const std::string& script) 
  : hasBegun_(false)
  , world_(ECS::World::createWorld())
  , script_(script) {

  // Register any systems
  world_->registerSystem(new PhysicsSystem());
}

// Destructor
Scene::~Scene() {
  world_->destroyWorld();
}

// Called when the scene is started
void
Scene::begin() {

  // Try to call the begin function from this scene's lua
  sol::protected_function onBegin = Game::lua["onBegin"];
	auto attempt = onBegin();
	if (!attempt.valid()) {
		sol::error err = attempt;
    if (Game::getDebugMode()) {
      printf("Error has occured: %s\n", err.what());
    }
  }

  // Flag that the scene has started
  hasBegun_ = true;
}

// Run this scene's script to register it's functions
void
Scene::registerFunctions() {
  Game::lua["world"] = world_;
  Game::lua.script_file(script_, sol::script_pass_on_error);
}

// When the screen is shown
void
Scene::showScene() {

  // Begin the scene if it hasn't yet
  if (!hasBegun_) {
    begin();
  }

  // Try to call the begin function from this scene's lua
  sol::protected_function onShow = Game::lua["onShow"];
	auto attempt = onShow();
	if (!attempt.valid()) {
		sol::error err = attempt;
    if (Game::getDebugMode()) {
      printf("Error has occured: %s\n", err.what());
    }
  }
}

// When the screen is hidden
void
Scene::hideScene() {
  sol::protected_function onHide = Game::lua["onHide"];
	auto attempt = onHide();
	if (!attempt.valid()) {
		sol::error err = attempt;
    if (Game::getDebugMode()) {
      printf("Error has occured: %s\n", err.what());
    }
  }
}

// Update the game every frame
void
Scene::update(const sf::Time& dt) {

  // Call scene's update script
  sol::protected_function onUpdate = Game::lua["onUpdate"];
	auto attempt = onUpdate(dt);
	if (!attempt.valid()) {
		sol::error err = attempt;
    if (Game::getDebugMode()) {
      printf("Error has occured: %s\n", err.what());
    }
  }

  // Update the ECS
  world_->update(dt);
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
    world_->emit<DebugRenderPhysicsEvent>({window});
  }
}

// Handle keypresses
void
Scene::handleEvent(const sf::Event& event) {

  // Call scene's update script
  sol::protected_function onWindowEvent = Game::lua["onWindowEvent"];
	auto attempt = onWindowEvent(event);
	if (!attempt.valid()) {
		sol::error err = attempt;
    if (Game::getDebugMode()) {
      printf("Error has occured: %s\n", err.what());
    }
  }
}

// When the game is quit
void
Scene::quit() {
  sol::protected_function onQuit = Game::lua["onQuit"];
	auto attempt = onQuit();
	if (!attempt.valid()) {
		sol::error err = attempt;
    if (Game::getDebugMode()) {
      printf("Error has occured: %s\n", err.what());
      printf("Terminating program anyway.\n");
    }
    Game::terminate();
  }
}
