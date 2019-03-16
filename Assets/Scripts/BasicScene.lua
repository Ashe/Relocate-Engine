-- BasicScene.lua
-- Lua file for the default scene

function spawnBox(x, y, size)
  box = createEntity()
  boxTrans = assignTransform(box)
  boxBody = assignRigidBody(box)
  boxTrans.position = Vector2f.new(x, y)
  boxBody.bodyDef = BodyDef.new()
  boxBody.bodyDef.type = Physics_DynamicBody
  boxFixture = FixtureDef.new()
  boxFixture.shape = BoxShape(size, size)
  boxFixture.density = 1
  boxFixture.friction = 0.3
  boxBody:addFixtureDef(boxFixture)
end

-- When the scene is shown for the first time
function onBegin()
  size = getWindowSize(getWindow())

  usePhysicsSystem()

  print("Spawning ground")
  ground = createEntity(world)
  groundTrans = assignTransform(ground)
  groundBody = assignRigidBody(ground)
  groundTrans.position = Vector2f.new(size.x * 0.5, size.y * 0.9)
  groundBody.bodyDef = BodyDef.new()
  groundFixture = FixtureDef.new()
  groundFixture.shape = BoxShape(size.x, 0.1)
  groundBody:addFixtureDef(groundFixture)
end

-- Every scene tick
function onUpdate(dt)
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

    -- Spawn box on left click
    if ev.mouseButton.button == MouseButton_Left then
      spawnBox(ev.mouseButton.x, ev.mouseButton.y, 0.5)
    elseif ev.mouseButton.button == MouseButton_Right then
      spawnBox(ev.mouseButton.x, ev.mouseButton.y, 1.5)
    end
  end

end
