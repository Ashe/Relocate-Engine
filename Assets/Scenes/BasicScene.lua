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
  sprite.size = Vector2f.new(size.x * 0.8, 10)
  sprite.origin = Vector2f.new(0.5, 0)
  sprite:setSprite("BoxTexture")
  local fixture = FixtureDef.new()
  fixture:setShape(LineShape(- size.x * 0.4, 0, size.x * 0.4, 0))
  groundBody:addFixture(fixture)

  -- Spawn the player
  print("Spawning player")
  player = World:createEntity()
  local possession = player:assignPossession()
  local camera = player:assignCamera()
  local stats = player:assignStats()
  local movement = stats.movement
  movement.movementSpeed = 300
  movement.canSprint = true
  movement.sprintSpeedMult = 2
  movement.canJump = true
  movement.canFly = false
  movement.flightSpeed = 100
  local combat = stats.combat
  combat.maxHealth = 100
  combat.deleteAfterAnimation = false
  local sprite = player:assignSprite()
  sprite.size = Vector2f.new(32, 32)
  sprite.scale = Vector2f.new(4.2, 4.2)
  sprite:setSprite("MageTexture")
  sprite:addAnimation("idle", "GenericIdle")
  sprite:addAnimation("walk", "GenericWalk")
  sprite:addAnimation("death", "GenericDeath")
  sprite:playAnimation("idle", true)
  local abilities = player:assignAbilities()
  abilities:addAbility(0, "LaunchBox")
  abilities:addAbility(1, "Levitate")
  abilities:addAbility(2, "Flight")
  local playerTrans = player:assignTransform()
  playerTrans.position = Vector2f.new(size.x * 0.5, size.y * 0.5)
  local playerBody = player:assignRigidBody()
  local bodyDef = BodyDef.new()
  bodyDef.type = Physics_DYNAMICBODY
  bodyDef.isFixedRotation = true
  playerBody:instantiate(bodyDef)
  fixture = FixtureDef.new()
  fixture:setShape(BoxShape(64, 128))
  fixture.density = 100
  fixture.friction = 10
  playerBody:addFixture(fixture)
  playerBody:makeGroundSensor()
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
