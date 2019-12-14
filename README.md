# Relocate Engine

Indie games like [Darkest Dungeon](https://www.darkestdungeon.com/) [Don't Starve](https://www.klei.com/games/dont-starve) interest me on many levels because of how they're built as well as how good they are. My favourite games tend to have their own 'feel', something which typically comes from having a custom game engine.

After reading [a blog by Elias Daler](https://eliasdaler.github.io/using-imgui-with-sfml-pt1/), I became interested in trying to make my own engine for creating a game featuring nice debugging and an ECS system. This was where the idea for the Relocate game came from - I wanted to play around with lua scripting and physics to make a little demo in my own engine. The idea wasn't to recreate the rendering, physics and ECS frameworks, but to simply put them all together for my own projects.

<video src="https://raw.githubusercontent.com/Ashe/Relocate-Engine/master/img/relocate-demo.webm" controls width="100%">
</video>

## Main Game
The core of the game is found in [`Game.h`](https://github.com/Ashe/Relocate-Engine/blob/master/src/Game.h) and handles the startup and shutdown of the executable. `Game` is a static class so that any other class used in the game has the ability to manipulate the game. The function `start()` contains the game loop and also handles multithreading by putting the render functionality on a seperate thread to the logic, if requested. The enum `Status` represents the current state of the game to ensure the following:

1. `Game` can only be `initialise()`d when it is completely not in use. This makes the game `Ready` to start.
2. `Game` isn't `start()`ed unless its `Status` is `Ready`, and will begin the main game loop and process the current `Scene` as long as one has been provided.
3. Any class can call `quit()` on `Game` to give it the `Quitting` status. This is so that all systems in the game can begin to stop what they're doing while they still can (ie, save game data). After the current scene has been `quit()`, `terminate()` gets called which attempts to close the window (if it hasn't been closed already) and sets the `Status` to `ShuttingDown`.
4. When `ShuttingDown`, the game loop is broken and all elements in the game stop processing fully.
5. After `start()` has finished execution, `Game` must be `shutdown()` to release all resources and set `Status` back to `Uninitialised`.

This handling of the game makes it easy to start and manage the game in [`main.cpp`](https://github.com/Ashe/Relocate-Engine/blob/master/src/main.cpp) like so:

```cpp
// Create and start the game
int main(int argc, char* argv[]) {

  // Set up whether we should multi thread or not
  bool multiThread = true, multiThreadSuccess = false;

#ifdef WIN32
  // Multithreading 'just works' on Windows
  multiThreadSuccess = true;
#endif

#ifdef linux
  // @TODO: More investigation is required to see if this will work on all Linux distros
  const int i = XInitThreads();
  if (i != 0) { multiThreadSuccess = true; }
  else { printf("Error: Failed to call XInitThreads, code %d\n", i); }
#endif

  // Initialise and start the game
  Game::initialise(sf::VideoMode(1920, 1080), "Game", multiThread && multiThreadSuccess);
  auto& scene = ResourceManager::getResource("BasicScene");
  if (scene.getType() == Resource::Type::SCENE) {
    Game::switchScene((Scene*)scene.get());
    Game::start();
  }
  Game::shutdown();
  return 0;
}
```

## ECS
For the ECS system I chose [Redxdev's single-header ECS](https://github.com/redxdev/ECS). A quick read of `README.md` is more than enough of an introduction to the framework and was easy to implement as I was using it from the beginning. This engine makes use of a [`Scene`](https://github.com/Ashe/Relocate-Engine/blob/master/src/Scene.h) class, where each `Scene` contains its own ECS `World` and all the `System`s that are enabled for the scene. Each `Scene` also contains it's own environment of `Lua` along with minimum functionality, so that `System`s in the `Scene`'s `World` are unique (in the sense that referring to `System` foo will only interact with the current `Scene`'s 'foo').

## Physics
[Box2D](https://box2d.org/) is used for the game's physics and [`PhysicsSystem.h`](https://github.com/Ashe/Relocate-Engine/blob/master/src/PhysicsSystem.h) was implemented with the ECS framework's `System` class. I learned that when implementing physics into a game engine, everything gets more complex if you use *variable* rather than *fixed* timestep. I accumulate delta time so that when enough time has elapsed and the timestep has been reached, the physics is simulated with `singleStep()`. The physics is interpolated in between steps so that the physics looks like it's updating as often as it can while also being accurate at least every timestep. The code is too long to place all in this blog, so instead [here's the link to the PhysicsSystem's `update()` function](https://github.com/Ashe/Relocate-Engine/blob/d0fa569f5127545030dd7e34ef1fe1513b093c1a/src/PhysicsSystem.cpp#L75).

## Lua
[Sol2](https://github.com/ThePhD/sol2) was used to integrate [Lua](https://www.lua.org/) with the engine. Functions and classes written in C++ can be created and manipulated through Lua, allowing Lua scripts to create functionality that works with the ECS framework. Every `Scene` and `System` registers functions to the `sol::State` so that they can be used in scripts and on-the-fly console commands.

```cpp
// The minimal setup function for providing knowledge of the world to Lua
void
Scene::registerFunctions() {

  // Set up environment
  lua_ = sol::environment(Game::lua, sol::create, Game::lua.globals());
  Script::registerSceneFunctions(lua_, world_);

  // Expose the world in the scene
  Game::lua["World"] = lua_;

  // Add to autocomplete
  Console::addCommand("[Class] World");
  Console::addCommand("World.createEntity");
}

// An example of how the PhysicsSystem adds commands
// This system also calls 'register' on any components to be registered
void
PhysicsSystem::registerPhysicsSystem(sol::environment& env, ECS::World* world) {

  // Create and install physics system
  env.set_function("usePhysicsSystem", [&env, world]() { 

    // ...

    // Allow the system's manipulation through lua
    env.set("Physics", newPS);
    env.new_usertype<PhysicsSystem>("PhysicsSystem",
      "gravity", sol::property(
        &PhysicsSystem::getGravity,
        &PhysicsSystem::setGravityVec),
      "setGravityMult", &PhysicsSystem::setGravityMult,
      "bodyCount", sol::property(
        [](const PhysicsSystem& self) { return self.world_.GetBodyCount(); }),
      "showHitboxes", sol::property(
        [](const PhysicsSystem& self) { return self.showRigidBodies_; },
        [](bool enable) { PhysicsSystem::showRigidBodies_ = enable; })
    );

    // Add global commands to auto complete
    Console::addCommand("[Class] Physics");
    Console::addCommand("Physics.gravity");
    Console::addCommand("Physics:setGravityMult");
    Console::addCommand("Physics.bodyCount");
    Console::addCommand("Physics.showHitboxes");

    // Allow the use of RigidBodies
    RigidBody::registerRigidBodyType(env, physicsWorld);
  });
}
```

Scripts are stored in the [`Assets` folder](https://github.com/Ashe/Relocate-Engine/tree/master/Assets) to be loaded during runtime with the exception of [`GameConfig.lua](https://github.com/Ashe/Relocate-Engine/blob/master/GameConfig.lua) which is used to run one-off commands and set the game up. In this case, `GameConfig.lua` defines the function `spawnCharacter` for spawning a generic character into a scene.

`Scene`s are created from `Lua` files - at the start of the game all assets are indexed and prepared for use by the [`ResouceManager`](https://github.com/Ashe/Relocate-Engine/blob/master/src/ResourceManager.h). When a `Scene` is requested, the script is read and a `Scene` is constructed using this data and is returned for the current and all future requests for the given [`Resource`](https://github.com/Ashe/Relocate-Engine/blob/master/src/Resource.cpp). [`BasicScene.lua`](https://github.com/Ashe/Relocate-Engine/blob/master/Assets/Scenes/BasicScene.lua) is an example of a `Scene` in `Lua`. Before any logic, all `System`s need to be declared in the `onBegin` function so that the relevant ECS `Component`s and functions used in these `System`s are defined and processed in the current `Scene`. After that, the script can hook onto some C++ functionality by defining functions such as `onUpdate` or `onWindowEvent`. After all the functions are defined, they need to be attached to a `Scene` that is created and returned at the end of the script as shown below:

```lua
-- BasicScene.lua
-- Lua file for the default scene

-- When the scene is shown for the first time
local function onBegin()

  -- onBegin() is just a formal function that runs AFTER the file is ready
  print("Executing begin")

  -- We define what systems we want to use
  -- These are parsed when the file is read and activated
  World.usePhysicsSystem()
  World.useControlSystem()
  World.useCameraSystem()
  World.useRenderSystem()
  World.useExpirySystem()
  World.useStatSystem()
  World.useCombatSystem()
  World.useSpellSystem()

  -- Get window size
  size = Game.displaySize

  -- Spawn bottom of map
  print("Spawning ground..")
  local ground = World:createEntity()
  local groundTrans = ground:assignTransform()
  local groundBody = ground:assignRigidBody()
  groundTrans.position = Vector2f.new(size.x * 0.5, size.y * 0.9)
  local sprite = ground:assignSprite()
  sprite.size = Vector2f.new(4000, 10)
  sprite.origin = Vector2f.new(0.5, 0)
  sprite:setSprite("BoxTexture")
  local fixture = FixtureDef.new()
  fixture:setShape(LineShape(-2000, 0, 2000, 0))
  groundBody:addFixture(fixture)

  -- Spawn the player
  print("Spawning player..")
  local pos = Vector2f.new(size.x * 0.5, size.y * 0.5)
  player = spawnCharacter(pos, "MageTexture", 100)
  local size = player:getSprite().size
  player:getSprite().spritesheetAnchor = Vector2i.new(0, size.y * 5)
  local possession = player:assignPossession()
  local camera = player:assignCamera()
  local stats = player:getStats()
  local combat = stats.combat
  combat.deleteAfterAnimation = false
  local abilities = player:assignAbilities()
  abilities:addAbility(0, "LaunchBox")
  abilities:addAbility(1, "Levitate")
  abilities:addAbility(2, "Flight")

  -- Make healthbar
  -- ...

  -- Spawn some example characters
  print("Spawning plebs..")
  for i = 0, 10 do
    local char = spawnCharacter(Vector2f.new(-2000 + (400 * i), Game.displaySize.y * 0.2), "OrcTexture", 50)
    local size = char:getSprite().size
    local rand = randomInt(0, 1)
    char:getSprite().spritesheetAnchor = Vector2i.new(0, size.y * rand * 5)
  end

  -- Spawn text here ...
end

-- On Update
local function onUpdate(dt)
  if (healthbarSprite ~= nil and player ~= nil and player:hasCombat()) then
    local length = player:getCombat().currentHealth
    if length < 0 then length = 0 end
    healthbarSprite.size.x = (length * (Game.displaySize.x * 0.3 - 50)) / 100
    healthbarSprite:updateSprite()
  end
end

-- On Window events
local function onWindowEvent(ev)

  -- Key pressed
  if ev.type == EventType_KeyPressed then

    -- Toggle debug on F1
    if ev.key.code == Key_F1 then
      Game.debug = not Game.debug

    -- Open console on F2
    elseif ev.key.code == Key_F2 then
      Game:openDevConsole()
    end
  end
end

-- Make and return the scene
local scene = Scene.new()
scene.onBegin = onBegin
scene.onUpdate = onUpdate
scene.onWindowEvent = onWindowEvent
return Resource_SCENE, "BasicScene", scene
```

Another about `Lua` is every asset, not just `Scene`s, use a `Lua` script to customise how they are set up. The `ResourceManager` firstly scans for these `.lua` files, and the actual resources such as images or sound would be referenced in these files. Here's an example of the texture script [`MageTexture.lua`](https://github.com/Ashe/Relocate-Engine/blob/master/Assets/Textures/Humanoids/MageTexture.lua):

```lua
-- MageTexture.lua
-- Spritesheet for the mage character
local mageTexture = Texture.new("Assets/Textures/Humanoids/Mage.png")
return Resource_TEXTURE, "MageTexture", mageTexture
```

And here's an example of the animation [`Walk.lua`](https://github.com/Ashe/Relocate-Engine/blob/master/Assets/Animations/Walk.lua):

```lua
-- Walk.lua
-- General walking animation
local walkAnimation = Animation.new()
for i = 0, 9 do
  walkAnimation:addFrame(IntRect.new(i * 32, 2 * 32, 32, 32))
end
return Resource_ANIMATION, "GenericWalk", walkAnimation
```

Finally, scripts themselves are assets too. The ability to create the boxes in the demo is actually a spell contained in the script [`LaunchBox.lua`](https://github.com/Ashe/Relocate-Engine/blob/master/Assets/Spells/LaunchBox.lua). The code is fairly long so I won't include that here.

## Debugging
Debugging functionality is the final aspect of this engine. [Imgui](https://github.com/ocornut/imgui) is perfect for building interfaces for tools like this. The debugging interface is shown when the `F1` key is pressed the console can be shown with `F2`; this is defined in the `BasicScene` snippet above and is not a forced keybinding.

Every `System` is allowed to have its own debug interface that can be found under the `View` menu in the main `Debug` window. The `Entity Viewer` and `Console` are always available, however. The `Entity Viewer` is perfect for viewing and modifying the components on the ECS framework's `Entities`. Just like how functions are registered to `Lua`, every `Component` also has it's own debugging function so that the information exposed when debugging is completely up to the programmer making new `Component`s easy to set up. The following snippet is from [`Transform.h`](https://github.com/Ashe/Relocate-Engine/blob/master/src/Transform.h), the component responsible for positioning `Entities` in the game world. The position and rotation of this component gets overwritten by many systems, such as the `PhysicsSystem`, and therefore doesn't need to be edited - it is just used to store the position the `Entity` should be rendered at. 

```cpp
// Shows the debug information to ImGui
void showDebugInformation() {
  ImGui::NextColumn();
  ImGui::Text("Position: %f, %f", position.x, position.y);
  ImGui::Text("Rotation: %f", rotation);
  ImGui::PushItemWidth(-1);
  ImGui::PopItemWidth();
  ImGui::NextColumn();
}
```

`System`s are a little trickier to debug as they can be so unique. The ECS framework contains a messaging system to generically call functions on any `System`, and so the `PhysicsSystem` can be debugged when receiving an `addDebugInfoEvent` in a message. Similarly, `System`s can add entries to the main menu with the `addDebugMenuEntryEvent` message. The `PhysicsSystem`'s debugging functions are featured below - this allows the user to see a count of how many physics bodies are in play and manipulate gravity using a slider.

```cpp
// Add physics entry to the main menu
void
PhysicsSystem::receive(ECS::World* w, const addDebugMenuEntryEvent& e) {
  ImGui::MenuItem("Physics System", NULL, &showPhysicsWindow_);
}

// Add information to debug window
void
PhysicsSystem::receive(ECS::World* w, const addDebugInfoEvent& e) {

  // Add to default window
  ImGui::Begin("Debug");
  ImGui::Text("Physics bodies: %d", world_.GetBodyCount());
  ImGui::End();

  // Make a physics window
  if (showPhysicsWindow_) {
    const auto gravityVec = getGravity();
    float gravity = gravityVec.y / 10.f;
    ImGui::Begin("Physics System", &showPhysicsWindow_);
    ImGui::DragFloat("Gravity", &gravity, 2.f);
    ImGui::End();
    if (gravity * 10.f != gravityVec.y) {
      setGravity(gravityVec.x, gravity * 10.f);
    }
  }
}
```

## Final thoughts
This engine was very fun to make and has been useful in a few projects such as the development of the test harness for my master's dissertation. It certainly isn't the best, but I'm now in the best position to make another engine if I decide to improve it.

For improvements, I'd definitely prefer to make `Game` a non-static class but still preserve the two-way communication that allows for a lua script to use `Game`s public members such as `quit()` and the debug flag. This would allow a developer to run two games from one `main.cpp`, possibly useful for testing the game with automation on multiple threads at once and collecting data without requiring having to restart the program or have lots of instances going at once.

Another improvement would be to clean the debugging interface and make more useful tools such as a level editor and a neater inspector for entities. The way `Scene`s are handled are also a little too black-boxy and it'd be nice to handle it more like `Godot` so that I don't need to decide between having all game levels dynamically loaded using one `Scene` and re-initialising all my `System`s again to load another level in a seperate `Scene`. These are things I wouldn't have really learned about if I hadn't have made this engine, and the results so far have been very satisfying.

![Screenshot](https://raw.githubusercontent.com/Ashe/Relocate-Engine/master/img/engine-preview.png)
