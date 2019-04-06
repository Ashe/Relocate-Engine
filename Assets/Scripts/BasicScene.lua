-- BasicScene.lua
-- Lua file for the default scene

local function spawnBox(x, y, size)
  local box = createEntity()
  lastSpawnedBox = box
  local boxTrans = box:assignTransform()
  local boxBody = box:assignRigidBody()
  spawnPos = Vector2f.new(x, y)
  boxTrans.position = spawnPos
  boxBody:instantiate(bodyDef)
  boxFixture.shape = BoxShape(size, size)
  boxBody:addFixture(boxFixture)
  return box
end

-- When the scene is shown for the first time
function onBegin()

  -- We define what systems we want to use
  -- These are parsed when the file is read and activated
  -- onBegin() is just a formal function that runs AFTER the file is ready
  usePhysicsSystem()
  useControlSystem()
  useCameraSystem()

  -- Get window size
  size = Game.displaySize

  -- Spawn a 'dead' RigidBody
  deadBody = createEntity():assignRigidBody()

  -- Create global vars
  bodyDef = BodyDef.new()
  bodyDef.type = Physics_DynamicBody
  boxFixture = FixtureDef.new()
  boxFixture.density = 500
  boxFixture.friction = 100

  -- Holding down mousekeys
  holdLeftMouse = false

  -- The joint used for right clicking
  joint = nil

  -- Spawn bottom of map
  print("Spawning ground")
  local ground = createEntity()
  local groundTrans = ground:assignTransform()
  local groundBody = ground:assignRigidBody()
  groundTrans.position = Vector2f.new(size.x * 0.5, size.y * 0.9)
  local fixture = FixtureDef.new()
  fixture.shape = LineShape(- size.x * 0.4, 0, size.x * 0.4, 0)
  groundBody:addFixture(fixture)

  -- Spawn the player
  print("Spawning player")
  player = createEntity()
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
function onUpdate(dt)

  -- Hold the current box
  if holdLeftMouse and boxToThrow then
    boxToThrow:getRigidBody():warpTo(spawnPos)
  end

  -- Pull the last spawned box
  if joint then
    joint.target = Vector2f.new(Game.mousePosition.x, Game.mousePosition.y)
  end
end

-- On Window events
function onWindowEvent(ev)

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
      boxToThrow = spawnBox(Game.mousePosition.x, Game.mousePosition.y, 50)
      holdLeftMouse = true

    -- Drag the last box on right click
    elseif ev.mouseButton.button == MouseButton_Right then
      if lastSpawnedBox then
        def = MouseJointDef.new()
        r = lastSpawnedBox:getRigidBody();
        def.target = r.location
        def:setBodyA(deadBody)
        def:setBodyB(r)
        def.maxForce = 500
        def.dampingRatio = 1
        joint = createMouseJoint(def)
      end
    end

  -- Mouse release
  elseif ev.type == EventType_MouseButtonReleased then

    -- On left release, 'fire' the object
    if ev.mouseButton.button == MouseButton_Left then
      holdLeftMouse = false
      local r = boxToThrow:getRigidBody()
      local throwScale = 25
      local impulse = Vector2f.new((Game.mousePosition.x - spawnPos.x) * throwScale, (Game.mousePosition.y - spawnPos.y) * throwScale)
      r:applyImpulseToCentre(impulse)
      boxToThrow = nil

    -- On right release, delete the joint
    elseif ev.mouseButton.button == MouseButton_Right then
      if joint then
        joint:destroy()
        joint = nil
      end
    end
  end
end
