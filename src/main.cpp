#include "Game.h"
#include "GameScreen.h"

// Create and start the game
int main(int argc, char* argv[]) {
  GameScreen* gs = new GameScreen();
  Game::start(sf::VideoMode(1920, 1080), "Game", gs);
}
