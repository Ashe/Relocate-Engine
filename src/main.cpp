#include "Game.h"
#include "ResourceManager.h"
#include "Scene.h"

#ifdef linux
#include <X11/Xlib.h>
#endif

// Create and start the game
int main(int argc, char* argv[]) {

  // Set up whether we should multi thread or not
  bool multiThread = true, multiThreadSuccess = false;

#ifdef WIN32
  // Multithreading 'just works' on Windows
  multiThreadSuccess = true;
#endif

#ifdef linux
  // @TODO: More investigation is required to see if this will work on all Linux distros
  const int i = XInitThreads();
  if (i != 0) { multiThreadSuccess = true; }
  else { printf("Error: Failed to call XInitThreads, code %d\n", i); }
#endif

  // Initialise and start the game
  Game::initialise(sf::VideoMode(1920, 1080), "Game", multiThread && multiThreadSuccess);
  auto& scene = ResourceManager::getResource("BasicScene");
  if (scene.getType() == Resource::Type::SCENE) {
    Game::switchScene((Scene*)scene.get());
    Game::start();
  }
  Game::shutdown();
  return 0;
}
