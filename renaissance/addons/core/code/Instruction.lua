require("AutoComplete.lua")
require("CVar.lua")
require("CVarManager.lua")
require("CCommand.lua")
require("CCommandManager.lua")

--[[!	\brief Instruction: either a Console Command or a CVar related instruction (set, print)
--]]

Instruction = {}

Instruction.instruction = nil
Instruction.parameters = {}
Instruction.cvarManager = false
Instruction.ccommandManager = false

function Instruction:New(info)
	local obj = {}
	setmetatable(obj, self)
	self.__index = self
	
	local function get(key)
		if info[key] then
			obj[key] = info[key]
		--not going to complain, since I need this to also work with the BindManager... I don't think this is going to be a problem?
		--elseif not self[key] then
		--	error("Instruction:New() called without "..key.."!", 3)
		end
	end
	
	get("instruction")
	get("parameters")
	get("cvarManager")
	get("ccommandManager")
	
	return obj
end

function Instruction:AutoComplete(instruction)
	local available = {} -- list of available commands and cvars
	for _, cvar in pairs(self.cvarManager.CVars) do
		table.insert(available, cvar.name)
	end
	for _, ccom in pairs(self.ccommandManager.CCommands) do
		table.insert(available, ccom.name)
	end
	
	return AutoComplete(instruction, available)
end

--[[!	\brief Executes a single instruction - usually called by Interpret()
		\return Whether the execution was successful (returns false if there were multiple possibilities, displays them if not silent)
--]]
function Instruction:Execute(silent)
	local silent = silent or false
	
	local instruction = self.instruction
	local parameters = self.parameters
	
	-- remove prefixed /
	if string.sub(instruction, 1, 1) == "/" then
		instruction = string.sub(instruction, 2)
	end
	
	-- "+"-prefixed commands have only one parameter: the amount (optional, defaults to 1)
	-- to stop a + command, you can either use "+name 0 [other parameters]" or "-name [other parameters]" - the latter is converted to the former
	local firstchar = instruction:sub(1, 1)
	if firstchar == "+" then
		if #parameters > 1 then
			jar.Logger.GetDefaultLogger():Warning("Commands starting with a plus may only have one (optional) parameter: the amount, ignoring additional parameters!")
			parameters = { parameters[1] }
		end
		parameters[1] = tonumber(parameters[1] or 1)
		if not parameters[1] then
			jar.Logger.GetDefaultLogger():Warning("Commands starting with a plus must have a number (or nothing) as the first parameter: the amount!")
			return false
		end
	elseif firstchar == "-" then
		instruction = "+" .. instruction:sub(2)
		if #parameters > 0 then
			jar.Logger.GetDefaultLogger():Warning("Commands starting with a minus must not have any arguments, ignoring!")
		end
		parameters = {0}
	end
	
	--auto complete the instruction
	
	local instruction, available = self:AutoComplete(instruction)
	
	--were there 0 results?
	if #available == 0 then
		--TODO: in MP use say. (Is this really desirable?)
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
			if instruction:lower() == name:lower() then
				unambiguous = true
				break
			end
		end
		
		if not unambiguous then
			-- display possibilities, do nothing else
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
	
	-- there was an unambiguous match
	-- that means there's either a CCommand of this name...
	if self.ccommandManager.CCommands[instruction:lower()] then
		--it's a command, call it with the right parameters
		self.ccommandManager.CCommands[instruction:lower()]:Execute(unpack(parameters))
		--done
		return true
	end
	-- ...or a CVar.
	if self.cvarManager.CVars[instruction:lower()] then
		if instruction:sub(1, 1) == "+" then
			jar.Logger.GetDefaultLogger():Warning("CVar names must not start with + or -!")
			return false
		end
		local cvar = self.cvarManager.CVars[instruction:lower()]
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
			
			if val == "=" then
				if (#parameters > 1) then
					val = parameters[2]
				else
					print("If you want to assign =, please write \"" .. instruction .. " = =\". Doing nothing.")
					return false
				end
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
					return true
				else
					cvar:SetValue(tonumber(val))
					return true
				end
			end
			
			if cvar.type == CVar.TYPES.FLOAT then
				if not IsNumber(val) then
					print(instruction .. " only accepts numbers!")
					return true
				else
					cvar:SetValue(tonumber(val))
					return true
				end
			end
			
			if cvar.type == CVar.TYPES.BOOLEAN then
				if not IsBool(val) then
					print(instruction .. " only accepts booleans ( true (1) / false (0) )!")
					return true
				else
					if val == "true" or val == "1" then
						cvar:SetValue(true)
					else
						cvar:SetValue(false)
					end
					return true
				end
			end
			
			jar.Logger.GetDefaultLogger():Warning("CVar " .. instruction .. " has an invalid type!")
			return false
			
			--done
		end
	end
	
	-- shouldn't get here (instruction can only be either CCommand or CVar)
	jar.Logger.GetDefaultLogger():Warning("Instruction:Execute(): Internal logic error, command \"" .. instruction .. "\" not executed")
	return false
end