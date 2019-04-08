-- FirstSpell.lua
-- Creates a spell in a modular way

local spellName = "testSpell"

local function castMajor(self)
  print("castMajor: " .. self.name)
end

local function castMinor(self)
  print("castMinor: " .. self.name)
end

local function releaseMajor(self)
  print("releaseMajor: " .. self.name)
end

local function releaseMinor(self)
  print("releaseMinor: " .. self.name)
end

local spell = Spell.new(spellName)
spell.onCastMajor = castMajor
spell.onCastMinor = castMinor
spell.onReleaseMajor = releaseMajor
spell.onReleaseMinor = releaseMinor

print("Returning made spell: " .. spellName)
return spell
