-- BoxTexture.lua
-- A single texture used for the basic boxes in the game
-- @NOTE: It will be better to eventually use one big texture for multiple sprites
-- .. but we don't have enough things to apply sprites to yet

local boxTexture = Texture.new("Assets/Textures/Box/00.png")
return Resource_TEXTURE, "BoxTexture", boxTexture
