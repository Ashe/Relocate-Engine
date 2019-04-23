-- Flight.lua
-- Enables flight (Demonstration of self-buffing)

local function toggleFlight(self)
  if self:hasMovement() then
    local s = self:getStats()
    s.movement.canFly = not s.movement.canFly
    if s.movement.canFly then 
      print("FLIGHT ENABLED")
    else 
      print("FLIGHT DISABLED") 
    end
    s.movement.flightSpeed = 1000
  end
end

-- Make and return spell
local name = "Flight"
local spell = Spell.new()
spell.name = name
spell.onCast= toggleFlight
return Resource_SPELL, name, spell
