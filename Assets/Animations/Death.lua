-- Death.lua
-- General death animation

local deathAnimation = Animation.new()
for i = 0, 9 do
  deathAnimation:addFrame(IntRect.new(i * 32, 9 * 32, 32, 32))
end
return Resource_ANIMATION, "GenericDeath", deathAnimation
