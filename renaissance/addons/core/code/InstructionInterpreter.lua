require("AutoComplete.lua")
require("CVar.lua")
require("CVarManager.lua")
require("CCommand.lua")
require("CCommandManager.lua")

-- by DavidManura, lua-users wiki
-- remove trailing and leading whitespace from string.
-- http://en.wikipedia.org/wiki/Trim_(8programming)
local function trim(s)
  -- from PiL2 20.4
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end


--[[!	\brief class for interpreting instruction strings, e.g. "+forward; say \"I am now moving \\\"forward\\\"!\""
--]]

InstructionInterpreter =
{
	CVarManager = nil,
	CCommandManager = nil,
}

function InstructionInterpreter:New(cvarManager, ccommandManager)
	if not cvarManager then
		error("InstructionInterpreter:New(): No CVarManager specified!", 2)
	end
	if not ccommandManager then
		error("InstructionInterpreter:New(): No CCommandManager specified!", 2)
	end
	obj = {}
	setmetatable(obj, self)
	self.__index = self
	obj.CVarManager = cvarManager
	obj.CCommandManager = ccommandManager
	return obj
end

--[[!	\brief Interprets (and executes) a given instruction set
		\param instructions A list of semicolon separated instructions, e.g. +forward or +left;say "awesome shit; strings may contain semicolons"
--]]
function InstructionInterpreter:Interpret(instructions)
	--parse the instructions character by character
	
	local currentInstruction = ""
	local currentParameters = {}
	local currentParameter = ""
	local readingInstruction = true -- whether we've not yet reached the parameters
	local inString = false
	
	local function ExecuteAndReset()
		--someone might add ; to his commands, in that case the last instruction is empty.
		if currentInstruction == "" then
			return
		end
		-- still have an unadded parameter?
		if currentParameter ~= "" then
			table.insert(currentParameters, trim(currentParameter))
		end
		self:Execute(currentInstruction, currentParameters)
		currentInstruction = ""
		currentParameters = {}
		currentParameter = ""
		readingInstruction = true
	end
	
	for i = 1, #instructions do
		local character = instructions:sub(i,i)
		local lastCharacter = instructions:sub(i-1,i-1)
		
		-- instruction string separator
		if character == ";" and not inString then
			ExecuteAndReset()
			
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
					currentInstruction = string.sub(currentInstruction, 1, -2) --remove the last letter, the backslash
					currentInstruction = currentInstruction .. "\""
				else
					currentParameter = string.sub(currentParameter, 1, -2) --remove the last letter, the backslash
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
	ExecuteAndReset()
end

--[[!	\brief Executes a single instruction - usually called by Interpret()
		\return Whether the execution was successful (returns false if there were multiple possibilities, displays them if not silent)
--]]
function InstructionInterpreter:Execute(instruction, parameters, silent)
	silent = silent or false
	if type(instruction) ~= "string" then
		jar.Logger.GetDefaultLogger():Warning("InstructionInterpreter:Execute() called with non-string instruction parameter!")
		return false
	end
	
	if type(parameters) ~= "table" then
		jar.Logger.GetDefaultLogger():Warning("InstructionInterpreter:Execute() called with non-table parameters parameter!")
		return false
	end
	if instruction == "" then
		--should not happen, but who knows
		return false
	end
	
	-- remove prefixed /
	if string.sub(instruction, 1, 1) == "/" then
		instruction = string.sub(instruction, 2)
	end
	
	-- -command cancels +command. since the name is +command, we need to change it. (but remember it's being cancelled)
	local isMinusCommand = false
	if string.sub(instruction, 1, 1) == "-" then
		instruction = "+" .. string.sub(instruction, 2)
		isMinusCommand = true
	end
	
	--auto complete the instruction
	
	local instruction, available = self:AutoComplete(instruction)
	
	--were there 0 results?
	if #available == 0 then
		--TODO: in MP use say.
		-- error, not found
		if not silent then
			print("No such command or cvar: " .. instruction)
		end
		-- done
		return false
	
	--were there multiple results?
	elseif #available > 1 then
		-- there may be multiple things starting like this, but it can still be unambigious, e.g. "quit" in case "quit" and "quitFast" exist
		local unambiguous = false
		for _, name in ipairs(available) do
			if instruction == name then
				unambiguous = true
				break
			end
		end
		
		if not unambiguous then
			-- display them
			if not silent then
				for _, name in ipairs(available) do
					print(name)
				end
			end
			-- done
			return false
		end
		--else: unambiguous. 
	end
	
	--there was an unambiguous match
	if self.CCommandManager.CCommands[instruction] then
		--it's a command, call it with the right parameters
		if isMinusCommand then
			self.CCommandManager.CCommands[instruction]:OnStop(unpack(parameters))
		else
			self.CCommandManager.CCommands[instruction]:OnStart(unpack(parameters))
		end
		--done
		return true
	end
	if self.CVarManager.CVars[instruction] then
		local cvar = self.CVarManager.CVars[instruction]
		if isMinusCommand then
			jar.Logger.GetDefaultLogger:Warning("There's a CVar whose name starts with a +. That's bad. Shouldn't happen.")
			return false
		end
		--it's a cvar, see if there's a parameter
		if #parameters == 0 then
			--nope, let's output the value then.
			print(cvar:ToString())
			-- or should I do self.CVarManager.CVars[instruction]:Print() ?
			--done
			return true
		else
			--there is - this is an assignment
			
			local val = parameters[1] -- this should be assigned
			
			if (val == "=" and (#parameters > 2 )) or (val ~= "=" and (#parameters > 1) ) then
				print("Ignoring additional arguments, if you want to assign a string containing spaces, please use quotes (e.g.: " .. instruction .. " \"contains spaces\")")
			end
			
			if val == "=" and (#parameters > 1) then
				val = parameters[2]
			else
				print("If you want to assign =, please write \"" .. instruction .. " = =\". Doing nothing.")
				return false
			end
			
			--verify type
			
			local function IsNumber(str)
				if string.match(str, "%d*%.?%d+") == str and str ~= "" then
					return true
				else
					return false
				end
			end
			
			local function IsInt(str)
				if string.match(str, "%d+") == str and str ~= "" then
					return true
				else
					return false
				end
			end
			
			local function IsBool(str)
				if str == "true" or str == "false" or str == "0" or str == "1" then
					return true
				else
					return false
				end
			end
			
			if cvar.type == CVar.TYPES.ANY or cvar.type == CVar.TYPES.STRING then
				cvar:SetValue(val)
				return true
			end
			
			if cvar.type == CVar.TYPES.INT then
				if not IsInt(val) then
					print(instruction .. " only accepts integers (whole numbers)!")
				else
					cvar:SetValue(tonumber(val))
					return true
				end
			end
			
			if cvar.type == CVar.TYPES.FLOAT then
				if not IsNumber(val) then
					print(instruction .. " only accepts numbers!")
				else
					cvar:SetValue(tonumber(val))
					return true
				end
			end
			
			if cvar.type == CVar.TYPES.BOOLEAN then
				if not IsBoolean(val) then
					print(instruction .. " only accepts booleans ( true (1) / false (0) )!")
				else
					if val == "true" or val == "1" then
						cvar:SetValue(true)
					else
						cvar:SetValue(false)
					end
					return true
				end
			end
			
			jar.Logger.GetDefaultLogger:Warning("CVar " .. instruction .. " has an invalid type!")
			return false
			
			--done
		end
	end
	
	--shouldn't get here
	jar.Logger.GetDefaultLogger():Warning("InstructionInterpreter:Execute(): Internal logic error, command \"" .. instruction .. "\" not executed")
	return false
end

function InstructionInterpreter:AutoComplete(instruction)
	local available = {} -- list of available commands and cvars
	for name, _ in pairs(self.CVarManager.CVars) do
		table.insert(available, name)
	end
	for name, _ in pairs(self.CCommandManager.CCommands) do
		table.insert(available, name)
	end
	
	return AutoComplete(instruction, available)
end