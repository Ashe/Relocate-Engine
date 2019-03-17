-- BasicScene.lua
-- Lua file for the default scene

-- We define what systems we want to use
-- These are parsed when the file is read and activated
-- onBegin() is just a formal function that runs AFTER this file is ready
usePhysicsSystem()

bodyDef = BodyDef.new()
bodyDef.type = Physics_DynamicBody
boxFixture = FixtureDef.new()
boxFixture.density = 1
boxFixture.friction = 0.3

function spawnBox(x, y, size)
  box = createEntity()
  boxTrans = assignTransform(box)
  boxBody = assignRigidBody(box)
  boxTrans.position = Vector2f.new(x, y)
  print("Spawning box")
  boxBody:instantiate(bodyDef)
  boxFixture.shape = BoxShape(size, size)
  boxBody:addFixture(boxFixture)
  print("There are " .. physicsBodyCount() .. " bodies in the system")
  return box
end

-- When the scene is shown for the first time
function onBegin()

  --usePhysicsSystem()

  size = getWindowSize(getWindow())

  print("Spawning ground")
  ground = createEntity()
  groundTrans = assignTransform(ground)
  groundBody = assignRigidBody(ground)
  groundTrans.position = Vector2f.new(size.x * 0.5, size.y * 0.9)
  fixture = FixtureDef.new()
  fixture.shape = LineShape(- size.x * 0.5, 0, size.x * 0.5, 0)
  groundBody:addFixture(fixture)
end

-- Holding down right mouse
holdRightMouse = false

-- Coords
mouseX = 0
mouseY = 0

-- The last box that was spawned
lastBox = nil

-- Every scene tick
function onUpdate(dt)
  if holdRightMouse and lastBox then
    warpTo(lastBox, mouseX, mouseY)
  end
end

-- On Window events
function onWindowEvent(ev)

  if ev.type == EventType_KeyPressed then

    -- Toggle debug on F1
    if ev.key.code == Key_F1 then
      setDebugMode(not getDebugMode())

    -- Open console on F2
    elseif ev.key.code == Key_F2 then
      openDevConsole()
    end

  elseif ev.type == EventType_MouseButtonPressed then

    mouseX = ev.mouseButton.x
    mouseY = ev.mouseButton.y

    -- Spawn box on left click
    if ev.mouseButton.button == MouseButton_Left then
      lastBox = spawnBox(mouseX, mouseY, 0.5)

    -- Warp on right click
    elseif ev.mouseButton.button == MouseButton_Right then
      if (lastBox) then
        holdRightMouse = true
      end
    end

  elseif ev.type == EventType_MouseButtonReleased then
    if ev.mouseButton.button == MouseButton_Right then
      holdRightMouse = false
    end

  elseif ev.type == EventType_MouseMoved then
    mouseX = ev.mouseMove.x
    mouseY = ev.mouseMove.y
  end

end
