// Scripting.cpp
// Functions to be called in a script

#include "Scripting.h"

// Avoid cyclic dependancies
#include "Game.h"
#include "Transform.h"
#include "RigidBody.h"

////////////
// MACROS //
////////////

#define REGISTER_VECTOR(TYPE, COMP, NAME) \
Game::lua.new_usertype<TYPE>(NAME, \
  sol::constructors<TYPE(), TYPE(COMP, COMP)>(), \
  "x", &TYPE::x, \
  "y", &TYPE::y \
);

///////////////////////
// MANAGER FUNCTIONS //
///////////////////////

// Handle the initialisation of lua
void
Script::startLua() {
  printf("Initialising Lua..\n");
  Game::lua.open_libraries(sol::lib::base);

  // GAME FUNCTIONS
  // Control
  Game::lua.set_function("quit", &Game::quit);
  Game::lua.set_function("terminate", &Game::terminate);
  // Variables
  Game::lua.set_function("getWindow", &Game::getWindow);
  Game::lua.set_function("getDebugMode", &Game::getDebugMode);
  Game::lua.set_function("setDebugMode", &Game::setDebugMode);
  Game::lua.set_function("getFPS", &Game::getFPS);
  Game::lua.set_function("getStatus", &Game::getStatus);

  // SFML FUNCTIONS
  // Vectors
  REGISTER_VECTOR(sf::Vector2i, int, "Vector2i");
  REGISTER_VECTOR(sf::Vector2u, unsigned, "Vector2u");
  REGISTER_VECTOR(sf::Vector2f, float, "Vector2f");
  // Time
  Game::lua.set_function("asMicroseconds", &sf::Time::asMicroseconds);
  Game::lua.set_function("asMilliseconds", &sf::Time::asMilliseconds);
  Game::lua.set_function("asSeconds", &sf::Time::asSeconds);
  // RenderWindow
  Game::lua.set_function("getWindowSize", &sf::RenderWindow::getSize);

  // ECS FUNCTIONS
  // General
  Game::lua.set_function("createEntity", &ECS::World::create);
  // Transform
  Transform::registerFunctions();
  // RigidBody
  RigidBody::registerFunctions();
}

// Unregister the standard scene functionality
void
Script::unregisterSceneFunctions() {
  Game::lua.set_function("onBegin", &Funcs::emptyFunction);
  Game::lua.set_function("onUpdate", &Funcs::emptyUpdateFunction);
  Game::lua.set_function("onShow", &Funcs::emptyFunction);
  Game::lua.set_function("onHide", &Funcs::emptyFunction);
  Game::lua.set_function("onQuit", &Game::terminate);
}

///////////////////////
// TRIVIAL FUNCTIONS //
///////////////////////

// Empty functions for not-in-use things
void Script::Funcs::emptyFunction() {}
void Script::Funcs::emptyUpdateFunction(const sf::Time& dt) {}

////////////////////
// SFML FUNCTIONS //
////////////////////

// Make a Vector2i
sf::Vector2i makeVector2i(int i, int j) { return sf::Vector2i(i, j); }
