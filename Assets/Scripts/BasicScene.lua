-- BasicScene.lua
-- Lua file for the default scene

-- When the scene is shown for the first time
function onBegin()
  print("Getting Window Size")
  size = getWindowSize(getWindow())

  print("Making the ground Entity")
  ground = createEntity(world)
  print("|- Creating Transform Component")
  groundTrans = assignTransform(ground)
  print("|- Creating RigidBody Component")
  groundBody = assignRigidBody(ground)
  groundTrans.position = Vector2i.new(size.x * 0.5, size.y * 0.75)
  print("|- Adding body definition")
  groundBody.bodyDef = BodyDef.new()
  groundFixture = FixtureDef.new()
  groundFixture.shape = BoxShape(size.x, 10)
  print("|- Adding fixture definition\n")
  groundBody:addFixtureDef(groundFixture)

  print("Creating the dynamic box")
  box = createEntity(world)
  print("|- Creating Transform Component")
  boxTrans = assignTransform(box)
  print("|- Creating RigidBody Component")
  boxBody = assignRigidBody(box)
  boxTrans.position = Vector2i.new(size.x * 0.5, 0)
  print("|- Adding body definition")
  boxBody.bodyDef = BodyDef.new()
  print("|- Making the box dynamic")
  boxBody.bodyDef.type = Physics_DynamicBody
  boxFixture = FixtureDef.new()
  boxFixture.shape = BoxShape(2, 2)
  boxFixture.density = 20
  boxFixture.friction = 0.3
  print("|- Adding fixture definition\n")
  boxBody:addFixtureDef(boxFixture)
end

-- Every scene tick
function onUpdate(dt)
end
