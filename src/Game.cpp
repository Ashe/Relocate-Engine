// Game.cpp
// This is a static, high level manager for the game's cycle

#include "Game.h"
#include "Scene.h"
#include "Config.h"

// Initialise static members
sf::RenderWindow Game::window_ = sf::RenderWindow();
bool Game::multiThread_ = false;
std::mutex Game::windowMutex_;
bool Game::debug_ = false;
Game::Status Game::status_ = Game::Status::Uninitialised;
Scene* Game::currentScene_ = nullptr;
unsigned Game::fps_ = 0;

// Initialise the game without starting the loop
void
Game::initialise(const sf::VideoMode& mode, const std::string& title, bool multiThread) {

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

  // Flag whether we are in multithreaded mode
  multiThread_ = multiThread;
  printf("Running in %s mode.\n", multiThread_ ? "multithreaded" : "standard");

  // Initialise Guile and ensure it works
  Script::startGuile();
  SCM success = scm_c_eval_string("(testGuile)");
  if (!scm_boolean_p(success) || success == SCM_BOOL_F) {
    printf("Error: Cannot initialise Guile correctly.\n"); 
    return;
  }

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
    printf("Error: Cannot start application.\n"); 
    return;
  }

  // We are now running the game
  status_ = Game::Status::Running;

  // Create a clock for measuring deltaTime
  sf::Clock clock_;

  // FPS variables
  sf::Clock fpsClock_;
  unsigned fpsFrame_ = 0;

  // Call the begin function before the loop starts
  begin();

  // Pointer to the render thread if necessary
  std::thread renderThread(Game::handleRenderThread);

  // Start a thread to render the game
  if (multiThread_) {
    window_.setActive(false);
    renderThread.detach();
  }

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

    // Prepare to collect input events
    bool process = true;
    std::vector<sf::Event> events;

    // If multithreading, gain access to window quickly
    if (multiThread_) { process = windowMutex_.try_lock(); }
    sf::Event e;

    // If we're allowed to process data
    if (process) {

      // Collect events
      while (window_.pollEvent(e)) {
        events.push_back(e);
      }

      // Disable the lock, we don't need the window
      if (multiThread_) { 
        windowMutex_.unlock(); 
      }

      // Resume processing events
      for (auto ev : events) {
        
        // React to each event
        switch (ev.type) {
          
          // Forward keypresses to the game
          case sf::Event::KeyPressed:
          case sf::Event::KeyReleased:
          case sf::Event::MouseButtonPressed:
          case sf::Event::MouseButtonReleased:
            handleEvent(ev);
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
    }

    // Update the game
    update(elapsed_);

    // Render every frame after updating
    if (!multiThread_) {
      render();
    }
    else {
      std::this_thread::yield();
    }
  }

  // Wait for render thread to finish
  if (multiThread_ && renderThread.joinable()) {
    renderThread.join();
  }

  // Quit the game and exit program
  shutdown();
  printf("Exiting..\n");
}

// Called just before the main loop starts
void
Game::begin() {

  // Begin the current scene when the game starts
  if (currentScene_ != nullptr) {
    currentScene_->begin();
  }
}

// Called every frame, returns true when game should end
void
Game::update(const sf::Time& dt) {

  // Update the screen if the pointer is set
  if (currentScene_ != nullptr) {
    currentScene_->update(dt);
  }
}

// Call the render() function from a seperate thread
void
Game::handleRenderThread() {

  // Easy out
  if (Game::getStatus() < Game::Status::Running) return;

  // Continually render the game while
  while (Game::getStatus() < Game::Status::ShuttingDown) {
    windowMutex_.lock();
    Game::render();
    windowMutex_.unlock();
    std::this_thread::yield();
  }
}

// Render the game every frame, after update
void
Game::render() {

  // Clear the window for rendering
  window_.clear();

  // Render the game if pointer is set
  if (currentScene_ != nullptr) {
    currentScene_->render(window_);
  }

  // Render everything in the screen
  window_.display();
}

// Respond to any key or mouse related events
void
Game::handleEvent(const sf::Event& event) {

  // Feed the screen the input
  if (currentScene_ != nullptr) {
    currentScene_->handleEvent(event);
  }
}

// Release resources before the game closes
void
Game::quit() {
  printf("Quitting game..\n");

  // Signal that we're quitting the game
  status_ = Game::Status::Quitting;

  // Allow the current screen to halt termination
  if (currentScene_) {
    currentScene_->quit();
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
  delete currentScene_;
}

// Change to the new screen
void 
Game::switchScene(Scene* scene) {

  // Switch away from old screen
  if (currentScene_ != nullptr) {
    currentScene_->hideScene();
  }

  // Change the screen to be rendered
  currentScene_ = scene;

  // Run any logic for showing screen
  if (currentScene_ != nullptr) {
    currentScene_->hideScene();
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

// Set debug mode
void
Game::setDebugMode(bool enable) {
  debug_ = enable;
  printf("Debug mode %s.\n", enable ? "enabled" : "disabled");
}

// Get debug mode
bool
Game::getDebugMode() {
  return debug_;
}
