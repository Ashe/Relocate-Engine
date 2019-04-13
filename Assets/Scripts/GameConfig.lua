-- GameConfig.lua
-- This file is required in order for the program to run
-- You can use this space to create global functions

print("Running GameConfig.lua")

-- Simply enable debug from the very beginning
Game.debug = true

-- Load some other lua scripts
-- @NOTE: This is not the way to do this, this is just 
-- to expose spells to global scope before 
-- resource manager has the functionality to load spells
_, _, leftSpell = dofile("Assets/Spells/LaunchBox.lua")
_, _, rightSpell = dofile("Assets/Spells/Levitate.lua")
