-- Flight.lua
-- Enables flight (Demonstration of self-buffing)

local function toggleFlight(self)
  if self:hasMovement() then
    local m = self:getMovement()
    m.canFly = not m.canFly
    if m.canFly then 
      print("FLIGHT ENABLED")
    else 
      print("FLIGHT DISABLED") 
    end
    m.flightSpeed = 500
  end
end

-- Make and return spell
local name = "Flight"
local spell = Spell.new()
spell.name = name
spell.onCast= toggleFlight
return Resource_SPELL, name, spell
