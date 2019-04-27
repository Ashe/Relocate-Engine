-- GameConfig.lua
-- This file is required in order for the program to run
-- You can use this space to create global functions

print("Running GameConfig.lua")

-- Simply enable debug from the very beginning
Game.debug = false

-- Convenience function for spawning a character
function spawnCharacter(pos, texture, hp)
  char = World:createEntity()
  local stats = char:assignStats()
  local movement = stats.movement
  movement.movementSpeed = 300
  movement.canSprint = true
  movement.sprintSpeedMult = 2
  movement.canJump = true
  movement.canFly = false
  movement.flightSpeed = 100
  local combat = stats.combat
  combat.maxHealth = hp
  combat.deathDelay = 1
  local sprite = char:assignSprite()
  sprite.size = Vector2f.new(32, 32)
  sprite.scale = Vector2f.new(4.2, 4.2)
  sprite:setSprite(texture)
  sprite:addAnimation("idle", "GenericIdle")
  sprite:addAnimation("walk", "GenericWalk")
  sprite:addAnimation("death", "GenericDeath")
  sprite:playAnimation("idle", true)
  local trans = char:assignTransform()
  trans.position = pos
  local body = char:assignRigidBody()
  local bodyDef = BodyDef.new()
  bodyDef.type = Physics_DYNAMICBODY
  bodyDef.isFixedRotation = true
  body:instantiate(bodyDef)
  fixture = FixtureDef.new()
  fixture:setShape(BoxShape(64, 128))
  fixture.density = 100
  fixture.friction = 10
  body:addFixture(fixture)
  body:makeGroundSensor()
  return char
end

local attemptMultiThread = true
return attemptMultiThread
