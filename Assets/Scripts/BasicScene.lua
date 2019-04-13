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

  -- Get window size
  size = Game.displaySize

  -- Spawn bottom of map
  print("Spawning ground")
  local ground = World:createEntity()
  local groundTrans = ground:assignTransform()
  local groundBody = ground:assignRigidBody()
  groundTrans.position = Vector2f.new(size.x * 0.5, size.y * 0.9)
  local fixture = FixtureDef.new()
  fixture.shape = LineShape(- size.x * 0.4, 0, size.x * 0.4, 0)
  groundBody:addFixture(fixture)

  -- Spawn the player
  print("Spawning player")
  player = World:createEntity()
  local possession = player:assignPossession()
  local camera = player:assignCamera()
  local movement = player:assignMovement()
  movement.movementSpeed = 300
  movement.canSprint = true
  movement.sprintSpeedMult = 2
  movement.canJump = true
  movement.canFly = false
  movement.flightSpeed = 100
  local playerTrans = player:assignTransform()
  playerTrans.position = Vector2f.new(size.x * 0.5, size.y * 0.5)
  local playerBody = player:assignRigidBody()
  local bodyDef = BodyDef.new()
  bodyDef.type = Physics_DynamicBody
  bodyDef.isFixedRotation = true
  playerBody:instantiate(bodyDef)
  fixture = FixtureDef.new()
  fixture.shape = BoxShape(32, 64)
  fixture.density = 100
  fixture.friction = 10
  playerBody:addFixture(fixture)
  playerBody:makeGroundSensor()
end

-- Every scene tick
local function onUpdate(dt)

  -- Hold the current box
  leftSpell:passiveCast(dt)
  rightSpell:passiveCast(dt)

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

  -- Mouse pressed
  elseif ev.type == EventType_MouseButtonPressed then

    -- Spawn box on left click
    if ev.mouseButton.button == MouseButton_Left then
      leftSpell:castMajor()

    -- Drag the last box on right click
    elseif ev.mouseButton.button == MouseButton_Right then
      rightSpell:castMajor()
    end

  -- Mouse release
  elseif ev.type == EventType_MouseButtonReleased then

    -- On left release, 'fire' the object
    if ev.mouseButton.button == MouseButton_Left then
      leftSpell:releaseMajor()

    -- On right release, delete the joint
    elseif ev.mouseButton.button == MouseButton_Right then
      rightSpell:releaseMajor()
    end
  end
end

-- Make and return the scene
local scene = Scene.new()
scene.onBegin = onBegin
scene.onUpdate = onUpdate
scene.onWindowEvent = onWindowEvent
return Resource_SCENE, "BasicScene", scene
