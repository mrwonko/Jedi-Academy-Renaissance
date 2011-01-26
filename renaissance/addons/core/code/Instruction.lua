require("AutoComplete.lua")
require("CVar.lua")
require("CVarManager.lua")
require("CCommand.lua")
require("CCommandManager.lua")

--[[!	\brief Instruction: either a Console Command or a CVar related instruction (set, print)
--]]

Instruction = 
{
	instruction = "",
	parameters = {},
}

function Instruction:New(info)
	if info.instruction == "" then
		--should not happen, but who knows
		error("Instruction:New() called with empty instruction!", 2)
	end
	
	if not info.CVarManager then
		error("Instruction:New() called without CVarManager info!", 2)
	end
	
	if not info.CCommandManager then
		error("Instruction:New() called without CCommandManager info!", 2)
	end
	
	local obj = info or {}
	
	if type(obj.parameters) ~= "table" then
		if obj.parameters ~= nil then
			jar.Logger.Warning("Instruction:New(): info.parameters is no table!")
		end
		obj.parameters = {}
	end
	
	setmetatable(obj, self)
	self.__index = self
	
	return obj
end

function Instruction:AutoComplete(instruction)
	local available = {} -- list of available commands and cvars
	for name, _ in pairs(self.CVarManager.CVars) do
		table.insert(available, name)
	end
	for name, _ in pairs(self.CCommandManager.CCommands) do
		table.insert(available, name)
	end
	
	return AutoComplete(instruction, available)
end

function Instruction:Execute(silent)
	local silent = silent or false
	
	local instruction = self.instruction
	
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
			self.CCommandManager.CCommands[instruction]:OnStop(unpack(self.parameters))
		else
			self.CCommandManager.CCommands[instruction]:OnStart(unpack(self.parameters))
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
		if #self.parameters == 0 then
			--nope, let's output the value then.
			print(cvar:ToString())
			-- or should I do self.CVarManager.CVars[instruction]:Print() ?
			--done
			return true
		else
			--there is - this is an assignment
			
			local val = self.parameters[1] -- this should be assigned
			
			if (val == "=" and (#self.parameters > 2 )) or (val ~= "=" and (#self.parameters > 1) ) then
				print("Ignoring additional arguments, if you want to assign a string containing spaces, please use quotes (e.g.: " .. instruction .. " \"contains spaces\")")
			end
			
			if val == "=" and (#self.parameters > 1) then
				val = self.parameters[2]
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