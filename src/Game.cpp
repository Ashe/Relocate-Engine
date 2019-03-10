// Game.cpp
// This is a static, high level manager for the game's cycle

#include "Game.h"
#include "Config.h"

// Initialise static members
sf::RenderWindow Game::window_ = sf::RenderWindow();
Game::Status Game::status_ = Game::Status::Uninitialised;
Screen* Game::currentScreen_ = nullptr;
unsigned Game::fps_ = 0;

// Initialise the game without starting the loop
void
Game::initialise(const sf::VideoMode& mode, const std::string& title) {

  // Check that we haven't already initialised
  if (status_ != Game::Status::Uninitialised) {
    printf("Error: Cannot initialise application - already running.\n"); 
    return;
  }

  // Print opening message
  printf("Launching Application %d.%d.%d..\n", 
    Build_VERSION_MAJOR, 
    Build_VERSION_MINOR, 
    Build_VERSION_TWEAK);

  // Create window
  window_.create(mode, title);

  // Flag that the game is ready to start
  status_ = Game::Status::Ready;
}

// Initiate the game loop
void
Game::start() {

  // Check that the game is ready to start
  if (status_ != Game::Status::Ready) {
    printf("Error: Cannot start application - already running.\n"); 
    return;
  }

  // Create a clock for measuring deltaTime
  sf::Clock clock_;

  // FPS variables
  sf::Clock fpsClock_;
  unsigned fpsFrame_ = 0;

  // Main game loop while window is open
  while (status_ < Game::Status::ShuttingDown) {
    
    // Get the time since last tick
    sf::Time elapsed_ = clock_.restart();

    // Calculate FPS
    if (fpsClock_.getElapsedTime().asSeconds() >= 1.0f) {
      fps_ = fpsFrame_;
      fpsFrame_ = 0;
      fpsClock_.restart();
    }
    ++fpsFrame_;

    // Poll all events
    sf::Event event_;
    while (window_.pollEvent(event_)) {
      
      // React to each event
      switch (event_.type) {
        
        // Forward keypresses to the game
        case sf::Event::KeyPressed:
        case sf::Event::KeyReleased:
        case sf::Event::MouseButtonPressed:
        case sf::Event::MouseButtonReleased:
          handleEvent(event_);
          break;
        
        // Forward resize events to the setup singleton
        case sf::Event::Resized:
          break;

        // Exit the game if you close the window
        case sf::Event::Closed:
          quit();
          break;

        // Otherwise
        default: break;
      }
    }


    //@TODO: Remove this
    window_.clear();

    // Update the game
    update(elapsed_);

    // Render every frame after updating
    render();
  }

  // Quit the game and exit program
  shutdown();
  printf("Exiting..\n");
}

// Called every frame, returns true when game should end
void
Game::update(const sf::Time& dt) {

  // Update the screen if the pointer is set
  if (currentScreen_ != nullptr) {
    currentScreen_->update(dt);
  }
}

// Render the game every frame, after update
void
Game::render() {

  // Clear the window for rendering
  //window_.clear();

  // Render the game if pointer is set
  if (currentScreen_ != nullptr) {
    currentScreen_->render(window_);
  }

  // Render everything in the screen
  window_.display();
}

// Respond to any key or mouse related events
void
Game::handleEvent(const sf::Event& event) {
  printf("Received some kind of event.\n");

  // Feed the screen the input
  if (currentScreen_ != nullptr) {
    currentScreen_->handleEvent(event);
  }
}

// Release resources before the game closes
void
Game::quit() {
  printf("Quitting game..\n");

  // Signal that we're quitting the game
  status_ = Game::Status::Quitting;

  // Allow the current screen to halt termination
  if (currentScreen_) {
    currentScreen_->quit();
  }
  else {
    terminate();
  }
}

// Called from within application to close it down
void
Game::terminate() {

  // Set the game's status to break game loop
  status_ = Game::Status::ShuttingDown;

  // Close the window, exiting the game loop
  window_.close();
}

// Free resources before program closes
void
Game::shutdown() {
  status_ = Game::Status::Uninitialised;
  printf("Releasing resources..\n");
  delete currentScreen_;
}

// Change to the new screen
void 
Game::switchScreen(Screen* screen) {

  // Switch away from old screen
  if (currentScreen_ != nullptr) {
    currentScreen_->hideScreen();
  }

  // Change the screen to be rendered
  currentScreen_ = screen;

  // Run any logic for showing screen
  if (currentScreen_ != nullptr) {
    currentScreen_->showScreen();
  }
}

// Get a pointer to the game's window
sf::RenderWindow*
Game::getWindow() {
  return &window_;
}

// Get FPS of application
unsigned
Game::getFPS() {
  return fps_;
}

// Get status of application
Game::Status
Game::getStatus() {
  return status_;
}
