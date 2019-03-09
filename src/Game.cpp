// Game.cpp
// This is a static, high level manager for the game's cycle

#include "Game.h"
#include "Config.h"

// Initialise static members
sf::RenderWindow Game::window_ = sf::RenderWindow();
Screen* Game::currentScreen_ = nullptr;

// Initiate the game loop
void
Game::start(const sf::VideoMode mode, const std::string& title) {

  // Print opening message
  printf("Launching Application %d.%d.%d..\n", 
    Build_VERSION_MAJOR, 
    Build_VERSION_MINOR, 
    Build_VERSION_TWEAK);

  // Create window
  window_.create(mode, title);

  // Create a clock for measuring deltaTime
  sf::Clock clock_;

  // FPS variables
  sf::Clock fpsClock_;
  unsigned int fpsFrame_ = 0;
  unsigned int fps_ = 0;

  // Main game loop while window is open
  while (window_.isOpen()) {
    
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
          window_.close();
          break;

        // Otherwise
        default: break;
      }
    }

    // Update the game
    update(fps_, elapsed_);

    // Render every frame after updating
    render();
  }

  // Quit the game and exit program
  quit();
  printf("Exiting..\n");
}

// Called every frame, returns true when game should end
void
Game::update(const unsigned int fps, const sf::Time& dt) {

  // Update the screen if the pointer is set
  if (currentScreen_ != nullptr)
    currentScreen_->update(dt);
}

// Render the game every frame, after update
void
Game::render() {

  // Clear the window for rendering
  window_.clear();

  // Render the game if pointer is set
  if (currentScreen_ != nullptr)
    currentScreen_->render(window_);

  // Render everything in the screen
  window_.display();
}

// Respond to any key or mouse related events
void
Game::handleEvent(const sf::Event& event) {
  printf("Received some kind of event.\n");

  // Feed the screen the input
  if (currentScreen_ != nullptr)
    currentScreen_->handleEvent(event);
}

// Release resources before the game closes
void
Game::quit() {
  printf("Releasing resources..\n");

  // Hide current screen and quit
  if (currentScreen_) {
    currentScreen_->hideScreen();
    currentScreen_->quit();
  }
}

// Change to the new screen
void 
Game::switchScreen(Screen& screen) {

  // Switch away from old screen
  if (currentScreen_ != nullptr)
    currentScreen_->hideScreen();

  // Change the screen to be rendered
  currentScreen_ = &screen;

  // Run any logic for showing screen
  currentScreen_->showScreen();
}

// Get a pointer to the game's window
sf::RenderWindow*
Game::getWindow() {
  return &window_;
}
