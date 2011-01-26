require("Instruction.lua")

--[[!	\brief InstructionBuffers store Instructions (CCommands and CVar stuff) and handle their execution
--]]

InstructionBuffer =
{
	instructions = {},
}

local function ToInstructions(instructionstring)
	--TODO. Copy from InstructionInterpreter.
end

function InstructionBuffer:New(instructionstring)
	if type(instructionstring) ~= "string" then
		jar.Logger.GetDefaultLogger():Warning("CommandBuffer:New(): no commandstring supplied!")
	end
	local obj = {}
	setmetatable(obj, self)
	self.__index = self
	
	obj.instructions = ToInstructions(instructionstring)
	
	return obj
end

--TODO: Exectution stuff.