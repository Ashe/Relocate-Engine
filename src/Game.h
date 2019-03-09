// Game.h
// This is a static, high level manager for the game's cycle

#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include "Screen.h"

class Game {
  public:

    // Start the game, calling update and render loops
    static void start(const sf::VideoMode m, const std::string& title);

    // Change the screen that is used and rendered
    static void switchScreen(Screen& screen);

    // Get a pointer to the game's window
    static sf::RenderWindow* getWindow();

  private:

    // SFML Window
    static sf::RenderWindow window_;

    // Screen management
    static Screen* currentScreen_;

    // Main game loop 
    static void update(const unsigned int fps, const sf::Time& dt);

    // Render the game
    static void render();

    // Handle keypresses etc
    static void handleEvent(const sf::Event& event);

    // Release resources on quit
    static void quit();
};

#endif
