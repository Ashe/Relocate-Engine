-- Levitate.lua
-- Levitates stuff

-- Variables for this spell
local nullbody = nil
local joint = nil

-- Pick up an object
function beginLevitation(self)
  if lastSpawnedBox ~= nil then

    -- Make a null body if necessary
    if nullbody == nil then
      nullbody = Scene:createEntity():assignRigidBody()
    end

    -- Use null body in creation of joint
    local def = MouseJointDef.new()
    local r = lastSpawnedBox:getRigidBody();
    def.target = r.location
    def:setBodyA(nullbody)
    def:setBodyB(r)
    def.maxForce = 500
    def.dampingRatio = 1
    joint = Scene.createMouseJoint(def)
  end
end

-- Change the destination for levitation
function updateDestination(self, dt)
  if joint then
    joint.target = Vector2f.new(Game.mousePosition.x, Game.mousePosition.y)
  end
end

-- Put down an object
function endLevitation(self)
  if joint then
    joint:destroy()
    joint = nil
  end
end

-- Make and return spell
local spell = Spell.new("Levitate")
spell.onCastMajor = beginLevitation
spell.onPassiveCast = updateDestination
spell.onReleaseMajor = endLevitation
return Resource_SPELL, spell.name, spell
