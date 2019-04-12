-- GameConfig.lua
-- This file is required in order for the program to run
-- You can use this space to create global functions

print("Running GameConfig.lua")

-- Simply enable debug from the very beginning
Game.debug = true

-- Load some other lua scripts
_, _, leftSpell = dofile("Assets/Scripts/LaunchBox.lua")
_, _, rightSpell = dofile("Assets/Scripts/Levitate.lua")
