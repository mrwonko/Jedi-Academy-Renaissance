require("Instruction.lua")

--[[!	\brief InstructionBuffers store Instructions (CCommands and CVar stuff) and handle their execution
--]]

InstructionBuffer = {}

InstructionBuffer.cvarManager = false
InstructionBuffer.ccommandManager = false
InstructionBuffer.waittime = 0 --time until the next command will be executed

function InstructionBuffer:New(instructionstring, cvarManager, ccommandManager)
	local obj =
	{
		instructions = {} --don't rename, used e.g. by BindManager
	}
	setmetatable(obj, self)
	self.__index = self
	
	if cvarManager then
		obj.cvarManager = cvarManager
	elseif not self.cvarManager then
		error("InstructionBuffer:New(): Called without a valid cvarManager!")
	end
	
	if ccommandManager then
		obj.ccommandManager = ccommandManager
	elseif not self.ccommandManager then
		error("InstructionBuffer:New(): Called without a valid ccommandManager!")
	end
	
	if type(instructionstring) ~= "string" then
		jar.Logger.GetDefaultLogger():Warning("CommandBuffer:New(): no commandstring supplied!")
		if DEBUG then jar.Logger.GetDefaultLogger():Warning(debug.traceback()) end
		return nil
	else
		obj:AddInstructions(instructionstring)
	end
	return obj
end
-- by DavidManura, lua-users wiki
-- remove trailing and leading whitespace from string.
-- http://en.wikipedia.org/wiki/Trim_(8programming)
local function trim(s)
  -- from PiL2 20.4
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

--[[!	\brief Adds Instructions based on a string of instructions
		\param instructionstring A string of semicolon separated instructions, e.g. +forward or +left;say "awesome shit; strings may contain semicolons"
--]]
function InstructionBuffer:AddInstructions(instructionstring)
	local currentInstruction = ""
	local currentParameters = {}
	local currentParameter = ""
	local readingInstruction = true -- whether we've not yet reached the parameters
	local inString = false
	
	local function InsertAndReset()
		--someone might add ; to his commands, in that case the last instruction is empty.
		if currentInstruction == "" then
			return
		end
		-- still have an unadded parameter?
		if currentParameter ~= "" then
			table.insert(currentParameters, trim(currentParameter))
		end
		table.insert(self.instructions, Instruction:New{instruction = currentInstruction, parameters = currentParameters})
		currentInstruction = ""
		currentParameters = {}
		currentParameter = ""
		readingInstruction = true
	end
	
	for i = 1, #instructionstring do
		local character = instructionstring:sub(i,i)
		local lastCharacter = instructionstring:sub(i-1,i-1)
		
		-- instruction string separator
		if character == ";" and not inString then
			InsertAndReset()
			
		-- parameter separator
		elseif (character == " " or character == "\t") and not inString then
			if not readingInstruction then --reading a parameter?
				if currentParameter ~= "" then --to allow multiple spaces
					table.insert(currentParameters, trim(currentParameter))
					currentParameter = ""
				end
			elseif currentInstruction ~= "" then
				readingInstruction = false
			end
		
		--string
		elseif character == "\"" then
			-- escaped?
			if lastCharacter == "\\" then
				if readingInstruction then
					currentInstruction = currentInstruction:sub(1, -2) --remove the last letter, the backslash
					currentInstruction = currentInstruction .. "\""
				else
					currentParameter = currentParameter:sub(1, -2) --remove the last letter, the backslash
					currentParameter = currentParameter .. "\""
				end
			else -- not escaped
				inString = not inString
			end
		
		-- nothing special, just append it
		else
			if readingInstruction then
				currentInstruction = currentInstruction .. character
			else
				currentParameter = currentParameter .. character
			end
		end
	end
	InsertAndReset()
end

function InstructionBuffer:Done()
	return (#self.instructions == 0)
end

function InstructionBuffer:Update(deltaT)
	if self:Done() then
		jar.Logger.GetDefaultLogger():Warning("InstructionBuffer:Update() called on an empty buffer")
		if DEBUG then jar.Logger.GetDefaultLogger():Warning(debug.traceback()) end
		return
	end
	self.waittime = self.waittime - deltaT
	if self.waittime <= 0 then
		while #self.instructions > 0 do
			local inst = self.instructions[1]
			--remove the first element, it will now be handled
			table.remove(self.instructions, 1)
			if inst.instruction == "wait" then
				if #inst.parameters == 0 then
					print("wait requires a time, e.g. \"wait 5\"")
				else
					if #inst.parameters > 1 then
						jar.Logger.GetDefaultLogger():Log("wait takes only one argument, ignoring additional arguments.")
					end
					if inst.parameters[1]:match("%d+") ~= inst.parameters[1] then
						jar.Logger.GetDefaultLogger():Warning("wait needs a whole number argument, e.g. \"wait 5\"")
					else
						self.waittime = self.waittime + tonumber(inst.parameters[1])
						if self.waittime > 0 then
							break
						end
					end
				end
			else --not wait
				inst:Execute() --not silent, outputting errors/warnings is okay.
			end
		end
	end
	assert(self.waittime > 0 or self:Done())
end

--TODO: Exectution stuff.