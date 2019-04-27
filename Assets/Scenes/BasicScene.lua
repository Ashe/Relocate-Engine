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
  print("Creating healthbar..")
  local bar = World.createEntity()
  local trans = bar:assignTransform()
  trans.position = Vector2f.new(50, 50)
  local ui = bar:assignUIWidget()
  ui.anchor = Vector2f.new(-1, -1)
  healthbarSprite = bar:assignSprite()
  healthbarSprite:setSprite("HealthbarTexture")
  healthbarSprite.origin = Vector2f.new(0, 0)
  healthbarSprite.size = Vector2f.new(1000, 4) healthbarSprite.scale = Vector2f.new(1, 5)

  -- Spawn some plebs
  for i = 0, 10 do
    local char = spawnCharacter(Vector2f.new(-2000 + (400 * i), Game.displaySize.y * 0.2), "OrcTexture", 50)
    local size = char:getSprite().size
    local rand = randomInt(0, 1)
    char:getSprite().spritesheetAnchor = Vector2i.new(0, size.y * rand * 5)
  end
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
