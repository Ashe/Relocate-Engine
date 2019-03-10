#include "Game.h"
#include "GameScreen.h"

// Create and start the game
int main(int argc, char* argv[]) {
  Game::initialise(sf::VideoMode(1920, 1080), "Game");
  Game::switchScreen(new GameScreen());
  Game::start();
}
