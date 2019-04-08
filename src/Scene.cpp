// Scene.cpp
// Logic more oriented to the game goes here

#include "Scene.h"

// Avoid cyclic dependencies
#include "ControlSystem.h"

// Constructor
Scene::Scene(const std::string& script) 
  : hasBegun_(false)
  , world_(ECS::World::createWorld())
  , script_(script) {
}

// Destructor
Scene::~Scene() {
  world_->destroyWorld();
}

// Called when the scene is started
void
Scene::begin() {

  // Try to call the begin function from this scene's lua
  sol::protected_function onBegin = lua_["onBegin"];
	auto attempt = onBegin();
	if (!attempt.valid()) {
		sol::error err = attempt;
    Console::log("[Error] in Scene.begin():\n> %s", err.what());
  }

  // Flag that the scene has started
  hasBegun_ = true;
}

// Run this scene's script to register it's functions
void
Scene::registerFunctions() {

  // Set up environment
  lua_ = sol::environment(Game::lua, sol::create, Game::lua.globals());
  Game::lua.script_file(script_, lua_, sol::script_pass_on_error);
  Script::registerSceneFunctions(lua_, world_);
  Game::lua["Scene"] = lua_;

  // Save this scene's functions
  onBegin = lua_["onBegin"];
  onShow = lua_["onShow"];
  onHide = lua_["onHide"];
  onUpdate = lua_["onUpdate"];
  onWindowEvent = lua_["onWindowEvent"];
  onQuit = lua_["onQuit"];

  // Add to autocomplete
  Console::addCommand("[Class] Scene");
}

// When the screen is shown
void
Scene::showScene() {

  // Begin the scene if it hasn't yet
  if (!hasBegun_) {
    begin();
  }

  // Try to call the begin function from this scene's lua
  if (onShow.valid()) {
    auto attempt = onShow();
    if (!attempt.valid()) {
      sol::error err = attempt;
      Console::log("[Error] in Scene.showScene():\n> %s", err.what());
    }
  }
}

// When the screen is hidden
void
Scene::hideScene() {
  if (onHide.valid()) {
    auto attempt = onHide();
    if (!attempt.valid()) {
      sol::error err = attempt;
      Console::log("[Error] in Scene.hideScene():\n> %s", err.what());
    }
  }
}

// Update the game every frame
void
Scene::update(const sf::Time& dt) {

  // Call scene's update script
  if (onUpdate.valid()) {
    auto attempt = onUpdate(dt);
    if (!attempt.valid()) {
      sol::error err = attempt;
      Console::log("[Error] in Scene.update():\n> %s", err.what());
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

  // Send event to control system
  ControlSystem::handleInput(event);

  // Call scene's input script
  if (onWindowEvent.valid()) {
    auto attempt = onWindowEvent(event);
    if (!attempt.valid()) {
      sol::error err = attempt;
      Console::log("[Error] in Scene.handleEvent():\n> %s", err.what());
    }
  }
}

// When the game is quit
void
Scene::quit() {
  bool quitAnyway = !onQuit.valid();
  if (!quitAnyway) {
    auto attempt = onQuit();
    if (!attempt.valid()) {
      quitAnyway = true;
      sol::error err = attempt;
      Console::log("[Error] in Scene.quit():\n> %s", err.what());
    }
  }
  if (quitAnyway) {
    quitAnyway = false;
    Console::log("Terminating program.");
    Game::terminate();
  }
}

// Add entries to debug menu
void
Scene::addDebugMenuEntries() {
  world_->emit<addDebugMenuEntryEvent>({});
}

// Add information to the main IMGUI debug window
void
Scene::addDebugInfoToDefault() {
  world_->emit<addDebugInfoEvent>({});
}
