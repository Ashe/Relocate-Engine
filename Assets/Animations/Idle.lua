-- Idle.lua
-- General idle animation

local idleAnimation = Animation.new()
for i = 0, 9 do
  idleAnimation:addFrame(IntRect.new(i * 32, 5 * 32, 32, 32))
end
return Resource_ANIMATION, "GenericIdle", idleAnimation
