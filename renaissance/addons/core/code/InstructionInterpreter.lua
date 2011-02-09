require("InstructionBuffer.lua")
require("Instruction.lua")

--[[!	\brief class for interpreting instruction strings, e.g. "+forward; say \"I am now moving \\\"forward\\\"!\""
		\note will get updated every frame automatically (since it inherits jar.Updatable)
--]]

InstructionInterpreter = {}

InstructionInterpreter.cvarManager = false
InstructionInterpreter.ccommandManager = false

function InstructionInterpreter:New(cvarManager, ccommandManager)
	local obj = { instructionBuffers = {} }
	setmetatable(obj, self)
	self.__index = self

	if cvarManager then
		obj.cvarManager = cvarManager
		Instruction.cvarManager = cvarManager
		InstructionBuffer.cvarManager = cvarManager
	elseif not InstructionInterpreter.cvarManager then
		error("InstructionInterpreter:__init(): No CVarManager specified!", 2)
	end
	if ccommandManager then
		obj.ccommandManager = ccommandManager
		Instruction.ccommandManager = ccommandManager
		InstructionBuffer.ccommandManager = ccommandManager
	elseif not InstructionInterpreter.ccommandManager then
		error("InstructionInterpreter:__init(): No CCommandManager specified!", 2)
	end
	return obj
end

function InstructionInterpreter:Interpret(instructionstring)
	if type(instructionstring) ~= "string" then
		jar.Logger.GetDefaultLogger():Warning("InstructionInterpreter:Interpret(): Invalid parameter: no string!")
		if DEBUG then jar.Logger.GetDefaultLogger():Warning(debug.traceback()) end
		return false
	end
	if instructionstring == "" then
		return true
	end
	
	local buf = InstructionBuffer:New(instructionstring)
	if not buf then
		return false
	end
	
	-- instructionstring didn't contain any real instructions?
	if buf:Done() then
		return true
	end
	
	table.insert(self.instructionBuffers, buf)
	
	return true
end

function InstructionInterpreter:Update(deltaT)
	local newBuffers = {}
	local count = 0
	for _, buf in ipairs(self.instructionBuffers) do
		buf:Update(deltaT)
		count = count + 1
		if not buf:Done() then
			table.insert(newBuffers, buf)
		end
	end
	self.instructionBuffers = newBuffers
end