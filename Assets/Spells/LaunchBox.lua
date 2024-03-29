-- LaunchBox.lua
-- Spell which spawns a box and fires it

-- Variables for this spell
local spawnPos = nil
local boxToThrow = nil
forceBoxSize = nil
boxSize = 1

-- Helper function for spawning the box
local function spawnBox(x, y, scale)
  local box = World.createEntity()
  lastSpawnedBox = box
  local boxTrans = box:assignTransform()
  local sprite = box:assignSprite()
  sprite.size = Vector2f.new(60, 60)
  sprite.scale = Vector2f.new(scale, scale)
  sprite:setSprite("BoxTexture")
  sprite:updateSprite()
  local spriteSize = sprite.size
  local boxBody = box:assignRigidBody()
  local bodyDef = BodyDef.new()
  bodyDef.type = Physics_DYNAMICBODY
  local boxFixture = FixtureDef.new()
  boxFixture.density = 500
  boxFixture.friction = 100
  spawnPos = Vector2f.new(x, y)
  boxTrans.position = spawnPos
  boxBody:instantiate(bodyDef)
  boxFixture:setShape(BoxShape(60 * scale, 60 * scale))
  boxBody:addFixture(boxFixture)
  return box
end

-- Spawn a box on spell cast
local function createBox()
  local size = boxSize
  if forceBoxSize ~= nil and forceBoxSize > 0 then 
    size = forceBoxSize
  end
  boxToThrow = spawnBox(Game.mousePosition.x, Game.mousePosition.y, size)
  boxSize = boxSize + 0.5
  if boxSize > 3 then boxSize = 1 end
end

-- Hold it in place
local function holdBox()
  if boxToThrow ~= nil then
    boxToThrow:getRigidBody():warpTo(spawnPos)
  end
end

-- Fire on spell release
local function launchBox()
  if boxToThrow ~= nil then
    local throwScale = 25
    local impulse = Vector2f.new((Game.mousePosition.x - spawnPos.x) * throwScale, (Game.mousePosition.y - spawnPos.y) * throwScale)
    boxToThrow:getRigidBody():applyImpulseToCentre(impulse)
    boxToThrow = nil
  end
end

-- Make and return the spell
local name = "LaunchBox"
local spell = Spell.new()
spell.name = name
spell.onCast= createBox
spell.onPassive= holdBox
spell.onRelease= launchBox
return Resource_SPELL, name, spell
