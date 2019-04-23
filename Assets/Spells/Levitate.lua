-- Levitate.lua
-- Levitates stuff

-- Variables for this spell
local nullbody = nil
local joint = nil

-- Pick up an object
local function beginLevitation(self)
  if lastSpawnedBox ~= nil then

    -- Make a null body if necessary
    if nullbody == nil then
      nullbody = World:createEntity():assignRigidBody()
    end

    -- Use null body in creation of joint
    local def = MouseJointDef.new()
    local r = lastSpawnedBox:getRigidBody();
    def.target = r.location
    def:setBodyA(nullbody)
    def:setBodyB(r)
    def.maxForce = 500
    def.dampingRatio = 1
    joint = World.createMouseJoint(def)
  end
end

-- Change the destination for levitation
local function updateDestination(self, dt)
  if joint then
    joint.target = Vector2f.new(Game.mousePosition.x, Game.mousePosition.y)
  end
end

-- Put down an object
local function endLevitation(self)
  if joint then
    joint:destroy()
    joint = nil
  end
end

-- Make and return spell
local name = "Levitate"
local spell = Spell.new()
spell.name = name
spell.onCast= beginLevitation
spell.onPassive= updateDestination
spell.onRelease= endLevitation
return Resource_SPELL, name, spell
