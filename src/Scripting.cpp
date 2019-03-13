// Scripting.cpp
// Functions to be called in a script

#include "Scripting.h"

// Avoid cyclic dependancies
#include "Game.h"

// Register functions to Lua
void
Script::startLua() {
  printf("Initialising Lua..\n");
  Game::lua.set_function("testLua", &Script::testLua);
  Game::lua.set_function("quit", &Game::quit);
  Game::lua.set_function("terminate", &Game::terminate);
  Game::lua.set_function("getDebugMode", &Game::getDebugMode);
  Game::lua.set_function("setDebugMode", &Game::setDebugMode);
  Game::lua.set_function("getFPS", &Game::getFPS);
  Game::lua.set_function("getStatus", &Game::getStatus);
}

///////////////////////
// TRIVIAL FUNCTIONS //
///////////////////////

// Simply test Lua
bool Script::testLua() {
  printf("Testing Lua registered functions..\n");
  return true;
}
