// Game.cpp
// This is a static, high level manager for the game's cycle

#include "Game.h"
#include "Scene.h"
#include "Config.h"
#include "Scripting.h"

// Initialise static members
sf::RenderWindow* Game::window_ = nullptr;
sf::View Game::view = sf::View();
bool Game::multiThread_ = false;
std::mutex Game::windowMutex_;
bool Game::debug_ = false;
Game::Status Game::status_ = Game::Status::Uninitialised;
Scene* Game::currentScene_ = nullptr;
sol::state Game::lua;
sf::Vector2f Game::mousePosition_ = sf::Vector2f();
sf::Vector2f Game::displaySize_ = sf::Vector2f();
Console Game::console_;
bool Game::showConsole_ = false;
unsigned Game::fps_ = 0;
bool Game::isImguiReady_ = false;
std::queue<ImWchar> Game::queuedChars_ = std::queue<ImWchar>();

// Initialise the game without starting the loop
void
Game::initialise(const sf::VideoMode& mode, const std::string& title, bool multiThread) {

  // Enable console debugging
  Console::initialise(true);

  // Check that we haven't already initialised
  if (status_ != Game::Status::Uninitialised) {
    Console::log("[Error] Cannot initialise application - already running."); 
    return;
  }

  // Print opening message
  Console::log("Launching Application %d.%d.%d..", 
    Build_VERSION_MAJOR, 
    Build_VERSION_MINOR, 
    Build_VERSION_TWEAK);

  // Flag whether we are in multithreaded mode
  multiThread_ = multiThread;
  Console::log("Running in %s mode.", multiThread_ ? "multithreaded" : "standard");

  // Initialise Lua and ensure it works
  bool success = initialiseLua("Assets/Scripts/GameConfig.lua");
  if (success) { 
    Console::log("Lua successfully initialised."); 
  }
  else {
    Console::log("[Error] Cannot initialise Lua correctly."); 
    return;
  }

  // Create window and prepare view
  window_ = new sf::RenderWindow(mode, title);
  view = window_->getDefaultView();

  // Set up size of the window
  const auto size = window_->getSize();
  displaySize_ = sf::Vector2f(size.x, size.y);

  // Enable debugging functionality
  ImGui::SFML::Init(*window_);

  // Flag that the game is ready to start
  status_ = Game::Status::Ready;
}

// Initiate the game loop
void
Game::start() {

  // Check that the game is ready to start
  if (status_ != Game::Status::Ready) {
    Console::log("[Error] Cannot start application."); 
    return;
  }

  // We are now running the game
  status_ = Game::Status::Running;

  // Create a clock for measuring deltaTime
  sf::Clock clock_;

  // FPS variables
  sf::Clock fpsClock_;
  unsigned fpsFrame_ = 0;

  // Disable the window
  if (multiThread_) {
    window_->setActive(false);
  }

  // Pointer to the render thread if necessary
  std::thread* renderThread;

  // Start a thread to render the game
  if (multiThread_) {
    renderThread = new std::thread(Game::handleRenderThread);
    renderThread->detach();
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
      while (window_->pollEvent(e)) {
        events.push_back(e);
      }

      // Disable the lock, we don't need the window
      if (multiThread_) { 
        windowMutex_.unlock(); 
      }

      // Resume processing events
      for (auto ev : events) {

        // Begin closing the game on quit
        // Otherwise, pass the event to the scene
        if (ev.type != sf::Event::Closed) {
          handleEvent(ev);
        }
        else {
          quit();
        }
      }
    }

    // Update the game
    update(elapsed_);

    // Render every frame after updating
    if (!multiThread_) {
      if (status_ < Game::Status::ShuttingDown) {
        render();
      }
    }
    else {
      std::this_thread::yield();
    }
  }

  // Wait for render thread to finish
  if (multiThread_ && renderThread->joinable()) {
    renderThread->join();
    delete renderThread;
  }

  // Quit the game and exit program
  shutdown();
  Console::log("Exiting..");
}

// Initialise lua
bool 
Game::initialiseLua(const std::string& fp) {

  // Register basic SFML functions
  Script::startLua();

  // Register Game functionality
  Game::lua.set("Game", Game());
  Game::lua.new_usertype<Game>("Game",
    // Control functions
    "quit", &Game::quit,
    "terminate", &Game::terminate,
    "openDevConsole", &Game::openDevConsole,
    // Variables
    "window", sol::property(&Game::getWindow),
    "displaySize", sol::property(&Game::getDisplaySize),
    "debug", sol::property(
      &Game::getDebugMode,
      &Game::setDebugMode),
    "fps", sol::property(&Game::getFPS),
    "status", sol::property(&Game::getStatus),
    "mousePosition", sol::property(&Game::getMousePosition)
  );

  // Console Convenience functions
  Game::lua.set_function("quit", &Game::quit);

  // Allow use of the console
  Game::lua.set("Console", Console());
  Game::lua.new_usertype<Console>("Console",
    "log", &Console::log,
    "clear", &Console::clear,
    "outputToTerminal", sol::property(
      &Console::getOutputToTerminal,
      &Console::setOutputToTerminal)
  );

  // Console Convenience functions
  Game::lua.set_function("print", &Console::log);
  Game::lua.set_function("log", &Console::log);
  Game::lua.set_function("clear", &Console::clear);

  // Add some basic commands to auto complete
  Console::addCommand("quit");
  Console::addCommand("Console.outputToTerminal");

  // Tries to call the global config script
  // If this fails, lua is not working and cannot read files
	auto functional = Game::lua.script_file(fp, &sol::script_pass_on_error);
  return functional.valid();
}

// Called every frame, returns true when game should end
void
Game::update(const sf::Time& dt) {

  // Easy out
  if (window_ == nullptr) return;

  // Update mouse position every frame
  sf::Vector2i mousePixelCoords = sf::Mouse::getPosition(*window_);
  mousePosition_ = window_->mapPixelToCoords(mousePixelCoords);

  // Update the screen if the pointer is set
  if (currentScene_ != nullptr) {
    currentScene_->update(dt);
  }

  // Update IMGUI debug interfaces
  if (debug_ && !isImguiReady_) {

    // Pass queued characters to ImGui
    auto& io = ImGui::GetIO();
    while (!queuedChars_.empty()) {
      io.AddInputCharacter(queuedChars_.front());
      queuedChars_.pop();
    }

    // Update imgui (calls ImGui::NewFrame())
    ImGui::SFML::Update(mousePixelCoords, displaySize_, dt);

    // Create imgui interfaces
    Game::handleImgui();

    // Ready the IMGUI frame
    isImguiReady_ = true;
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
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
  }
}

// Render the game every frame, after update
void
Game::render() {

  // Easy out
  if (window_ == nullptr) return;

  // Clear the window for rendering
  window_->clear();

  // Render the game if pointer is set
  if (currentScene_ != nullptr) {
    window_->setView(view);
    currentScene_->render(*window_);
  }

  // Render IMGUI debug interface
  if (isImguiReady_) {
    ImGui::SFML::Render(*window_);
    isImguiReady_ = false;
  }

  // Render everything in the screen
  window_->display();
}

// Respond to any key or mouse related events
void
Game::handleEvent(const sf::Event& event) {

  // Adjust the viewport if window is resized
  if (event.type == sf::Event::Resized) {
    displaySize_ = sf::Vector2f(event.size.width, event.size.height);
    sf::FloatRect visibleArea(0.f, 0.f, displaySize_.x, displaySize_.y);
    Game::view = sf::View(visibleArea);
  }

  // Pass events to IMGUI debug interface
  bool passToGame = true;
  bool passToImgui = debug_;
  if (debug_) {

    // Get reference to IO
    auto& io = ImGui::GetIO();

    // Decide whether the game should get the event
    if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
      if (io.WantCaptureMouse) passToGame = false;
    }
    else if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
      if (io.WantCaptureKeyboard) passToGame = false;
    }
    else if (event.type == sf::Event::TextEntered && io.WantTextInput) {
      passToImgui = false;
      auto imguiChar = static_cast<ImWchar>(event.text.unicode);
      queuedChars_.push(imguiChar);
    }
  }

  // Handle imgui events
  if (passToImgui) { ImGui::SFML::ProcessEvent(event); }

  // Pass events to scene
  if (currentScene_ != nullptr && passToGame) {
    currentScene_->handleEvent(event);
  }
}

// Release resources before the game closes
void
Game::quit() {
  Console::log("Quitting game..");

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

  // Shut down IMGUI debug interface
  ImGui::SFML::Shutdown();

  // Shut down console debugging
  Console::shutdown();

  // Close the window, exiting the game loop
  if (window_ != nullptr) {
    window_->close();
    delete window_;
    window_ = nullptr;
  }
}

// Free resources before program closes
void
Game::shutdown() {
  status_ = Game::Status::Uninitialised;
  Console::log("Releasing resources..");
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
    currentScene_->registerFunctions();
    currentScene_->showScene();
  }
}

// Update imgui interfaces
void
Game::handleImgui() {

  // Set up IMGUI flags
  ImGuiWindowFlags flags = 0;
  flags |= ImGuiWindowFlags_MenuBar;

  // Standard debug info
  // @TODO: Remove call to imgui demo
  static bool showImguiDemo = false;
  ImGui::Begin("Debug", NULL, flags);
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("View")) {
      ImGui::MenuItem("Demo imgui", NULL, &showImguiDemo);
      ImGui::MenuItem("Console", NULL, &showConsole_);

      // Allow the scene to make entries to the view tab
      if (currentScene_ != nullptr) {
        currentScene_->addDebugMenuEntries();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }

  // Show ImGui demo on request
  if (showImguiDemo) { ImGui::ShowDemoWindow(&showImguiDemo); }

  // Show console
  if (showConsole_) { console_.create("Console", &showConsole_); }

  // Info
  ImGui::Spacing();
  ImGui::Text(std::string(
    "FPS: " + std::to_string(fps_)).c_str());
  ImGui::Text(std::string(
    "Window Size: " + 
    std::to_string((int)displaySize_.x) + "x" + 
    std::to_string((int)displaySize_.y)).c_str());

  // Camera
  const auto viewCentre = view.getCenter();
  ImGui::Text(std::string(
    "Camera Position: (" + 
    std::to_string((int)viewCentre.x) + "," + 
    std::to_string((int)viewCentre.y) + ")").c_str());

  // Mouse
  ImGui::Text(std::string(
    "Mouse Position: (" + 
    std::to_string((int)mousePosition_.x) + "," + 
    std::to_string((int)mousePosition_.y) + ")").c_str());
  ImGui::Spacing();

  // End default debug window
  ImGui::End();

  // Allow scene to make debug windows
  if (currentScene_ != nullptr) {
    currentScene_->addDebugInfoToDefault();
  }
}

// Open the dev console in the terminal
void
Game::openDevConsole() {
  showConsole_ = !showConsole_;
}

// Get a pointer to the game's window
const sf::RenderWindow*
Game::getWindow() {
  return window_;
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

// Get up to date mouse position
sf::Vector2f
Game::getMousePosition() {
  return mousePosition_;
}

// Get up to display size
sf::Vector2f
Game::getDisplaySize() {
  return displaySize_;
}

// Set debug mode
void
Game::setDebugMode(bool enable) {
  debug_ = enable;
  Console::log("Debug mode %s.", enable ? "enabled" : "disabled");
}

// Get debug mode
bool
Game::getDebugMode() {
  return debug_;
}
