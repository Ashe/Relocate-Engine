-- BasicScene.lua
-- Lua file for the default scene

-- When the scene is shown for the first time
local function onBegin()

  print("Executing begin")
  -- We define what systems we want to use
  -- These are parsed when the file is read and activated
  -- onBegin() is just a formal function that runs AFTER the file is ready
  World.usePhysicsSystem()
  World.useControlSystem()
  World.useCameraSystem()
  World.useSpriteSystem()
  World.useStatSystem()
  World.useCombatSystem()
  World.useSpellSystem()

  -- Get window size
  size = Game.displaySize

  -- Spawn bottom of map
  print("Spawning ground")
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
  print("Spawning player")
  player = spawnCharacter(Vector2f.new(size.x * 0.5, size.y * 0.5), "MageTexture", 100)
  local possession = player:assignPossession()
  local camera = player:assignCamera()
  local stats = player:getStats()
  local combat = stats.combat
  combat.deleteAfterAnimation = false
  local abilities = player:assignAbilities()
  abilities:addAbility(0, "LaunchBox")
  abilities:addAbility(1, "Levitate")
  abilities:addAbility(2, "Flight")

  -- Spawn some plebs
  for i = 0, 10 do
    spawnCharacter(Vector2f.new(-2000 + (400 * i), size.y * 0.9), "OrcTexture", 50)
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
