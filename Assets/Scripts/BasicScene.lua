-- BasicScene.lua
-- Lua file for the default scene

-- We define what systems we want to use
-- These are parsed when the file is read and activated
-- onBegin() is just a formal function that runs AFTER this file is ready
usePhysicsSystem()

-- Global vars
bodyDef = BodyDef.new()
bodyDef.type = Physics_DynamicBody
boxFixture = FixtureDef.new()
boxFixture.density = 20
boxFixture.friction = 500
ground = nil

spawnPos = nil

function spawnBox(x, y, size)
  box = createEntity()
  lastSpawnedBox = box
  boxTrans = box:assignTransform()
  boxBody = box:assignRigidBody()
  spawnPos = Vector2f.new(x, y)
  boxTrans.position = spawnPos
  print("Spawning box")
  boxBody:instantiate(bodyDef)
  boxFixture.shape = BoxShape(size, size)
  boxBody:addFixture(boxFixture)
  print("There are " .. physicsBodyCount() .. " bodies in the system")
  return box
end

-- When the scene is shown for the first time
function onBegin()

  -- Get window size
  size = getWindow():getSize()

  -- Spawn bottom of map
  print("Spawning ground")
  ground = createEntity()
  groundTrans = ground:assignTransform()
  groundBody = ground:assignRigidBody()
  groundTrans.position = Vector2f.new(size.x * 0.5, size.y * 0.9)
  fixture = FixtureDef.new()
  fixture.shape = LineShape(- size.x * 0.4, 0, size.x * 0.4, 0)
  groundBody:addFixture(fixture)
end

-- Holding down mousekeys
holdLeftMouse = false

-- Coords
mouseX = 0
mouseY = 0

-- The last box that was spawned
boxToThrow = nil
lastSpawnedBox = nil

-- The joint used for right clicking
joint = nil

-- Every scene tick
function onUpdate(dt)

  -- Hold the current box
  if holdLeftMouse and boxToThrow then
    boxToThrow:getRigidBody():warpTo(spawnPos)
  end

  -- Pull the last spawned box
  if joint then
    joint.target = Vector2f.new(mouseX, mouseY)
  end
end

-- On Window events
function onWindowEvent(ev)

  -- Key pressed
  if ev.type == EventType_KeyPressed then

    -- Toggle debug on F1
    if ev.key.code == Key_F1 then
      setDebugMode(not getDebugMode())

    -- Open console on F2
    elseif ev.key.code == Key_F2 then
      openDevConsole()
    end

  -- Mouse pressed
  elseif ev.type == EventType_MouseButtonPressed then

    -- Record mouse
    mouseX = ev.mouseButton.x
    mouseY = ev.mouseButton.y

    -- Spawn box on left click
    if ev.mouseButton.button == MouseButton_Left then
      boxToThrow = spawnBox(mouseX, mouseY, 50)
      holdLeftMouse = true

    -- Drag the last box on right click
    elseif ev.mouseButton.button == MouseButton_Right then
      if lastSpawnedBox then
        def = MouseJointDef.new()
        r = lastSpawnedBox:getRigidBody();
        def.target = r:getLocation()
        def:setBodyA(ground:getRigidBody())
        def:setBodyB(r)
        def.maxForce = 100
        def.dampingRatio = 1
        joint = createMouseJoint(def)
      end
    end

  -- Mouse release
  elseif ev.type == EventType_MouseButtonReleased then

    -- Record mouse
    mouseX = ev.mouseButton.x
    mouseY = ev.mouseButton.y

    -- On left release, 'fire' the object
    if ev.mouseButton.button == MouseButton_Left then
      holdLeftMouse = false
      throwScale = 1
      impulse = Vector2f.new((mouseX - spawnPos.x) * throwScale, (mouseY - spawnPos.y) * throwScale)
      boxToThrow:getRigidBody():applyImpulseToCentre(impulse)
      boxToThrow = nil

    -- On right release, delete the joint
    elseif ev.mouseButton.button == MouseButton_Right then
      if joint then
        joint:destroy()
        joint = nil
      end
    end

  -- Record where the mouse goes
  elseif ev.type == EventType_MouseMoved then
    mouseX = ev.mouseMove.x
    mouseY = ev.mouseMove.y
  end
end
