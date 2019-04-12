-- LaunchBox.lua
-- Spell which spawns a box and fires it

-- Variables for this spell
local spawnPos = nil
local boxToThrow = nil

-- Helper function for spawning the box
local function spawnBox(x, y, size)
  local box = World.createEntity()
  lastSpawnedBox = box
  local boxTrans = box:assignTransform()
  local boxBody = box:assignRigidBody()

  local bodyDef = BodyDef.new()
  bodyDef.type = Physics_DynamicBody
  local boxFixture = FixtureDef.new()
  boxFixture.density = 500
  boxFixture.friction = 100

  spawnPos = Vector2f.new(x, y)
  boxTrans.position = spawnPos
  boxBody:instantiate(bodyDef)
  boxFixture.shape = BoxShape(size, size)
  boxBody:addFixture(boxFixture)
  return box
end


-- Spawn a box on spell cast
local function createBox(self)
  if boxToThrow == nil then
    boxToThrow = spawnBox(Game.mousePosition.x, Game.mousePosition.y, 50)
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
