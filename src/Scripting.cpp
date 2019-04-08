// Scripting.cpp
// Functions to be called in a script

#include "Scripting.h"

// Avoid cyclic dependancies
#include "Game.h"
#include "Transform.h"
#include "Camera.h"
#include "RigidBody.h"
#include "Possession.h"

#include "CameraSystem.h"
#include "PhysicsSystem.h"
#include "ControlSystem.h"

////////////
// MACROS //
////////////

#define REGISTER_VECTOR(TYPE, COMP, NAME) \
Game::lua.new_usertype<TYPE>(NAME, \
  sol::constructors<TYPE(), TYPE(COMP, COMP)>(), \
  "x", &TYPE::x, \
  "y", &TYPE::y \
);

///////////////////////
// MANAGER FUNCTIONS //
///////////////////////

// Handle the initialisation of lua
void
Script::startLua() {
  Console::log("Initialising Lua..");
  Game::lua.open_libraries(sol::lib::base);

  // SFML FUNCTIONS
  // Events
  Funcs::registerEvents();
  // Vectors
  Funcs::registerVectors();
  // Time
  Game::lua.set_function("microseconds", &sf::microseconds);
  Game::lua.set_function("milliseconds", &sf::milliseconds);
  Game::lua.set_function("seconds", &sf::seconds);
  Game::lua.new_usertype<sf::Time>("Time",
    sol::constructors<sf::Time()>(),
    "asMicroseconds", &sf::Time::asMicroseconds,
    "asMilliseconds", &sf::Time::asMilliseconds,
    "asSeconds", &sf::Time::asSeconds
  );
  // RenderWindow
  Game::lua.new_usertype<sf::RenderWindow>("RenderWindow",
    "size", sol::property(&sf::RenderWindow::getSize)
    );

  // REGISTER ENTITY FUNCTIONS
  Game::lua.new_usertype<ECS::Entity>("Entity",
    "removeAllComponents", [](ECS::Entity& self) {self.removeAll();}
  );

  // COMPONENTS
  Transform::registerFunctions();
  Camera::registerFunctions();
  RigidBody::registerNonDependantFunctions();
  Possession::registerFunctions();
  Movement::registerFunctions();
}

// Register scene specific functions (EG. SYSTEMS FOR THE WORLD)
void
Script::registerSceneFunctions(sol::environment& env, ECS::World* world) {

  // Enable the creation and destruction of entities
  env.set_function("createEntity", &ECS::World::create, world);
  env.new_usertype<ECS::Entity>("Entity",
    "destroy", [world](ECS::Entity& self) { world->destroy(&self);}
  );

  // Register functions that 'turn on' systems in the world
  CameraSystem::registerCameraSystemFunctions(env, world);
  PhysicsSystem::registerPhysicsSystemFunctions(env, world);
  ControlSystem::registerControlSystemFunctions(env, world);

  // Add to autocomplete
  Console::addCommand("createEntity");
}

///////////////////////
// TRIVIAL FUNCTIONS //
///////////////////////

// Empty functions for not-in-use things
void Script::Funcs::emptyFunction() {}
void Script::Funcs::emptyUpdateFunction(const sf::Time& dt) {}
void Script::Funcs::emptyWindowEventFunction(const sf::Event& ev) {}

////////////////////
// SFML FUNCTIONS //
////////////////////

void Script::Funcs::registerVectors() {
  REGISTER_VECTOR(sf::Vector2i, int, "Vector2i");
  REGISTER_VECTOR(sf::Vector2u, unsigned, "Vector2u");
  REGISTER_VECTOR(sf::Vector2f, float, "Vector2f");
}

void Script::Funcs::registerEvents() {

  // Register main event type
  Console::log("Registering Event type");
  Game::lua.new_usertype<sf::Event>("Event",
    sol::constructors<sf::Event()>(),
    "type", &sf::Event::type,
    "key", &sf::Event::key,
    "text", &sf::Event::text,
    "size", &sf::Event::size,
    "mouseMove", &sf::Event::mouseMove,
    "mouseButton", &sf::Event::mouseButton,
    "mouseWheel", &sf::Event::mouseWheel,
    "mouseWheelScroll", &sf::Event::mouseWheelScroll,
    "joystickMove", &sf::Event::joystickMove,
    "joystickButton", &sf::Event::joystickButton,
    "joystickConnect", &sf::Event::joystickConnect,
    "touch", &sf::Event::touch,
    "sensor", &sf::Event::sensor
  );

  // Register the MouseMove type for mouse movement
  Console::log("Registering MouseMove type");
  Game::lua.new_usertype<sf::Event::MouseMoveEvent>("MouseMoveEvent",
    sol::constructors<sf::Event::MouseMoveEvent()>(),
    "x", &sf::Event::MouseMoveEvent::x,
    "y", &sf::Event::MouseMoveEvent::y
  );

  // Register the MouseButton type for mouse input
  Console::log("Registering MouseButton type");
  Game::lua.new_usertype<sf::Event::MouseButtonEvent>("MouseButtonEvent",
    sol::constructors<sf::Event::MouseButtonEvent()>(),
    "button", &sf::Event::MouseButtonEvent::button,
    "x", &sf::Event::MouseButtonEvent::x,
    "y", &sf::Event::MouseButtonEvent::y
  );

  // Register mouse buttons
  Console::log("Registering MouseButton values");
  Game::lua.set("MouseButton_Left", sf::Mouse::Button::Left);
  Game::lua.set("MouseButton_Right", sf::Mouse::Button::Right);
  Game::lua.set("MouseButton_Middle", sf::Mouse::Button::Middle);
  Game::lua.set("MouseButton_XButton1", sf::Mouse::Button::XButton1);
  Game::lua.set("MouseButton_XButton2", sf::Mouse::Button::XButton2);

  // Register the MouseWheel SCROLL type for mousewheel event
  Console::log("Registering MouseWheelScrollEvent type");
  Game::lua.new_usertype<sf::Event::MouseWheelScrollEvent>("MouseWheelScrollEvent",
    sol::constructors<sf::Event::MouseWheelScrollEvent()>(),
    "wheel", &sf::Event::MouseWheelScrollEvent::wheel,
    "delta", &sf::Event::MouseWheelScrollEvent::delta,
    "x", &sf::Event::MouseWheelScrollEvent::x,
    "y", &sf::Event::MouseWheelScrollEvent::y
  );

  // Register mouse wheels
  Console::log("Registering MouseWheel values");
  Game::lua.set("MouseWheel_VerticalWheel", sf::Mouse::Wheel::VerticalWheel);
  Game::lua.set("MouseWheel_HorizontalWheel", sf::Mouse::Wheel::HorizontalWheel);

  // Register the KeyEvent type for keyboard input
  Console::log("Registering KeyEvent type");
  Game::lua.new_usertype<sf::Event::KeyEvent>("KeyEvent",
    sol::constructors<sf::Event::KeyEvent()>(),
    "code", &sf::Event::KeyEvent::code,
    "alt", &sf::Event::KeyEvent::alt,
    "control", &sf::Event::KeyEvent::control,
    "shift", &sf::Event::KeyEvent::shift,
    "system", &sf::Event::KeyEvent::system
  );

  // Using https://www.sfml-dev.org/documentation/2.5.0/Event_8hpp_source.php
  // to gather all enum values
  const std::string eventLs[(int) sf::Event::EventType::Count] =
  {"Closed","Resized","LostFocus","GainedFocus","TextEntered","KeyPressed","KeyReleased","MouseWheelMoved","MouseWheelScrolled","MouseButtonPressed","MouseButtonReleased","MouseMoved","MouseEntered","MouseLeft","JoystickButtonPressed","JoystickButtonReleased","JoystickMoved","JoystickConnected","JoystickDisconnected","TouchBegan","TouchMoved","TouchEnded", 
"SensorChanged"};

  // Pass through all event enums
  Console::log("Registering EventType values");
  for (int i = 0; i < sf::Event::EventType::Count; ++i) {
    const std::string& str = eventLs[i];
    if (Game::getDebugMode()) Console::log("%d - Registering event: EventType_%s", i, str.c_str());
    Game::lua.set("EventType_" + str, (sf::Event::EventType) i);
  }

  // Using https://www.sfml-dev.org/documentation/2.5.0/Keyboard_8hpp_source.php
  // to gather all enum values
  const std::string keyLs[(int) sf::Keyboard::KeyCount + 1] =
  { "Unknown", "A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","Num0","Num1","Num2","Num3","Num4","Num5","Num6","Num7","Num8","Num9","Escape","LControl","LShift","LAlt","LSystem","RControl","RShift","RAlt","RSystem","Menu","LBracket","RBracket","Semicolon","Comma","Period","Quote","Slash","Backslash","Tilde","Equal","Hyphen","Space","Enter","Backspace","Tab","PageUp","PageDown","End","Home","Insert","Delete","Add","Subtract","Multiply","Divide","Left","Right","Up","Down","Numpad0","Numpad1","Numpad2","Numpad3","Numpad4","Numpad5","Numpad6","Numpad7","Numpad8","Numpad9","F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12","F13","F14","F15","Pause"};

  // Pass through all keys
  Console::log("Registering KeyButton values");
  for (int i = -1; i < sf::Keyboard::Key::KeyCount; ++i) {
    const std::string& str = keyLs[i + 1];
    if (Game::getDebugMode()) Console::log("%d - Registering character: Key_%s", i, str.c_str());
    Game::lua.set("Key_" + str, (sf::Keyboard::Key) i);
  }
}
