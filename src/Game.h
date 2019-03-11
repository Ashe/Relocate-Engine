// Game.h
// This is a static, high level manager for the game's cycle

#ifndef GAME_H
#define GAME_H

#include <thread>
#include <mutex>
#include <chrono>
#include <functional>
#include <SFML/Graphics.hpp>

#include "ECS.h"

// Forward declaration
class Scene;

class Game {
  public:

    // What the game is currently doing
    enum Status { Uninitialised, Ready, Running, Quitting, ShuttingDown };

    // Initialise the game window
    static void initialise(const sf::VideoMode& m, const std::string& title, bool multiThread = false);

    // Start the game, calling update and render loops
    static void start();

    // Change the screen that is used and rendered
    static void switchScene(Scene* scene);

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

    // Debug mode
    static bool getDebugMode();
    static void setDebugMode(bool enable);

  private:

    // Tracking what the game is currently doing
    static Status status_;

    // Whether we are multithreaded
    static bool multiThread_;

    // Enable debugging functionality
    static bool debug_;

    // Window to render to
    static sf::RenderWindow window_;

    // Mutex to protect the window for rendering
    static std::mutex windowMutex_;

    // Scene management
    static Scene* currentScene_;

    // FPS
    static unsigned fps_;

    // Start of the game loop
    static void begin();

    // Main game loop 
    static void update(const sf::Time& dt);

    // Multithread's render loop
    static void handleRenderThread();

    // Render the game
    static void render();

    // Handle keypresses etc
    static void handleEvent(const sf::Event& event);

    // Release resources before closing
    static void shutdown();
};

#endif
