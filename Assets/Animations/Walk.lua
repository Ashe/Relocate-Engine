-- Walk.lua
-- General walking animation

local walkAnimation = Animation.new()
for i = 0, 9 do
  walkAnimation:addFrame(IntRect.new(i * 32, 7 * 32, 32, 32))
end
return Resource_ANIMATION, "GenericWalk", walkAnimation
