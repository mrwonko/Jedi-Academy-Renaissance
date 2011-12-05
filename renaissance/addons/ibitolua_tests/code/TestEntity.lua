require "TestEuler.lua"
require "TestVector.lua"
require "TestTypeVerification.lua"

TestEntity = {}

local angles = TestEuler:New(0, 0, 0)
local vector = TestVector:New(0, 0, 0)

function TestEntity:New()
	local obj = {}
	setmetatable(obj, self)
	self.__index = self
	return obj
end

function TestEntity:GetAngles(...)
	CheckNumParam({...}, "Entity:GetAngles")
	return angles
end

function TestEntity:GetOrigin(...)
	CheckNumParam({...}, "Entity:GetOrigin")
	return vector
end

local validChannels =
{
	["CHAN_AUTO"] = true,
	["CHAN_LOCAL"] = true,
	["CHAN_WEAPON"] = true,
	["CHAN_VOICE"] = true,
	["CHAN_VOICE_ATTEN"] = true,
	["CHAN_VOICE_GLOBAL"] = true,
	["CHAN_ITEM"] = true,
	["CHAN_BODY"] = true,
	["CHAN_AMBIENT"] = true,
	["CHAN_LOCAL_SOUND"] = true,
	["CHAN_ANNOUNCER"] = true,
	["CHAN_LESS_ATTEN"] = true,
	["CHAN_MUSIC"] = true,
}
function TestEntity:PlaySound(channel, filename, ...)
	CheckNumParam({...}, "Entity:PlaySound")
	CheckString(filename, "filename", "Entity:PlaySound")
	if not validChannels[channel] then error("Entity:PlaySound: Invalid Channel", 2) end
end

function TestEntity:MoveTo(pos, time, ...)
	CheckNumParam({...}, "Entity:MoveTo")
	CheckVector(pos, "position", "Entity:MoveTo")
	CheckNumber(time, "time_ms", "Entity:MoveTo")
end

function TestEntity:RotateTo(angle, time, ...)
	CheckNumParam({...}, "Entity:RotateTo")
	CheckEuler(angle, "angle", "Entity:RotateTo")
	CheckNumber(time, "time_ms", "Entity:RotateTo")
end

function TestEntity:PlayRoff(filename, ...)
	CheckNumParam({...}, "Entity:PlayRoff")
	CheckString(filename, "filename", "Entity:PlayRoff")
end