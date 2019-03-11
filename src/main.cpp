#include "Game.h"
#include "Scene.h"

#ifdef linux
#include <X11/Xlib.h>
#endif

// Create and start the game
int main(int argc, char* argv[]) {

  bool multiThread = false;

#ifdef linux
  int i = XInitThreads();
  if (i != 0) {
    multiThread = true;
  }
  else {
    printf("Error: Failed to call XInitThreads, code %d\n", i);
  }
#endif

  Game::initialise(sf::VideoMode(1920, 1080), "Game", multiThread);
  Game::switchScene(new Scene());
  Game::start();
  return 0;
}
