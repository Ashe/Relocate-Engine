// Game.h
// This is a static, high level manager for the game's cycle

#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>

#include "ECS.h"
#include "Screen.h"

class Game {
  public:

    // What the game is currently doing
    enum Status { Uninitialised, Ready, Running, Quitting, ShuttingDown };

    // Initialise the game window
    static void initialise(const sf::VideoMode& m, const std::string& title);

    // Start the game, calling update and render loops
    static void start();

    // Change the screen that is used and rendered
    static void switchScreen(Screen* screen);

    // Get a pointer to the game's window
    static sf::RenderWindow* getWindow();

    // Get the FPS of the application
    static unsigned getFPS();

    // Get the status of the game
    static Status getStatus();

    // Tell the application to quit
    static void quit();

    // Signal that the application can now close safely
    static void terminate();

  private:

    // Tracking what the game is currently doing
    static Status status_;

    // SFML Window
    static sf::RenderWindow window_;

    // Screen management
    static Screen* currentScreen_;

    // FPS
    static unsigned fps_;

    // Main game loop 
    static void update(const sf::Time& dt);

    // Render the game
    static void render();

    // Handle keypresses etc
    static void handleEvent(const sf::Event& event);

    // Release resources before closing
    static void shutdown();
};

#endif
