-- LaunchBox.lua
-- Spell which spawns a box and fires it

-- Variables for this spell
local spawnPos = nil
local boxToThrow = nil
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
  boxFixture.shape = BoxShape(60 * scale, 60 * scale)
  boxBody:addFixture(boxFixture)
  return box
end

-- Spawn a box on spell cast
local function createBox(self)
  if boxToThrow == nil then
    boxToThrow = spawnBox(Game.mousePosition.x, Game.mousePosition.y, boxSize)
  end
end

-- Hold it in place
local function holdBox(self, dt)
  if boxToThrow ~= nil then
    boxToThrow:getRigidBody():warpTo(spawnPos)
  end
end

-- Fire on spell release
local function launchBox(self)
  if boxToThrow ~= nil then
    local throwScale = 25
    local impulse = Vector2f.new((Game.mousePosition.x - spawnPos.x) * throwScale, (Game.mousePosition.y - spawnPos.y) * throwScale)
    boxToThrow:getRigidBody():applyImpulseToCentre(impulse)
    boxToThrow = nil
  end
end

-- Make and return the spell
local spell = Spell.new("LaunchBox")
spell.onCastMajor = createBox
spell.onPassiveCast = holdBox
spell.onReleaseMajor = launchBox
return Resource_SPELL, spell.name, spell
