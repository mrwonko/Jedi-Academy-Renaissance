require("CVar.lua")
require("AutoComplete.lua")

CVarManager = {}

CVarManager.configfile = "config/cvars.lua"
-- name -> cvar
CVarManager.CVars = {}

function CVarManager:New()
	local obj = { CVars = {} }
	setmetatable(obj, self)
	self.__index = self
	return obj
end

--[[!	\brief Registers a new CVar, i.e. creates one unless it already exists (and is not usercreated)
		\param info see CVar:New()
--]]
function CVarManager:RegisterCVar(info)
	if type(info) ~= "table" then
		jar.Logger.GetDefaultLogger():Warning("CVarManager:RegisterCVar(): no table supplied!")
		return false
	end
	if type(info.name) ~= "string" then
		jar.Logger.GetDefaultLogger():Warning("CVarManager:RegisterCVar(): no valid CVar name supplied!")
		return false
	end
	if string.sub(info.name, 1, 1) == "+" or string.sub(info.name, 1, 1) == "-"  then
		jar.Logger.GetDefaultLogger():Warning("CVarManager:RegisterCVar() called with a CVar name starting with + or -! That's invalid.")
		return false
	end
	local nameLwr = string.lower(info.name)
	-- does such a cvar already exist?
	if self.CVars[nameLwr] then
		if not self.CVars[nameLwr].flags.userCreated then --not user created, i.e. created using RegisterCVar()
			jar.Logger.GetDefaultLogger():Warning("CVarManager:RegisterCVar(): duplicate registration of CVar " .. info.name .."! Ignoring second.")
			return false
		else
			-- CVar already exists, but it's user created, i.e. entered in console or in config.
			-- we can keep the value in that case, though we need to adjust the type. Keep in mind we may be using the default type though.
			local newVal = self.CVars[nameLwr]:ToType(info.type or CVar.type)
			-- ToType() may fail. in that case discard old value.
			if newVal then
				info.value = newVal
			else
				jar.Logger.GetDefaultLogger():Info("CVarManager:RegisterCvar(): Discarding old value of " .. info.name .. " as it can't be converted to the registered type.")
			end
		end
	end
	-- cvar does not exist yet or should be overwritten. so let's create it.
	if type(info.value) == "nil" and type(info.defaultValue) ~= "nil" then
		info.value = info.defaultValue
	end
	self.CVars[nameLwr] = CVar:New(info)
	return true
end

function CVarManager:SetCVar(cvarname, value)
	local nameLwr = string.lower(cvarname)
	-- do we have a cvar of that name?
	if self.CVars[nameLwr] then
		-- yes. set its value
		return self.CVars[nameLwr]:SetValue(value)
	else
		-- no. create one, flag it usercreated.
		if type(cvarname) ~= "string" then
			jar.Logger.GetDefaultLogger():Warning("CVarManager:SetCVar() called with invalid cvarname!")
			return false
		end
		if string.sub(cvarname, 1, 1) == "+" or string.sub(cvarname, 1, 1) == "-"  then
			jar.Logger.GetDefaultLogger():Warning("CVarManager:SetCVar() called with a cvarname starting with + or -! That's invalid.")
			return false
		end
		-- guess type from value's type
		local t
		if type(value) == "string" then
			t = CVar.TYPES.STRING
		elseif type(value) == "number" then
			t = CVar.TYPES.FLOAT
		elseif type(value) == "boolean" then
			t = CVar.TYPES.BOOLEAN
		else
			jar.Logger.GetDefaultLogger():Warning("CVarManager:SetCVAR() called with an invalid value!")
			return false
		end
		self.CVars[nameLwr] = CVar:New{
			name = cvarname,
			value = value,
			defaultValue = value,
			description = "unregistered/user created - no description available",
			type = t
		}
		if not self.CVars[nameLwr] then
			return false
		end
		self.CVars[nameLwr].flags.userCreated = true
		return true
	end
end

function CVarManager:GetCVar(cvarname)
	cvarname = string.lower(cvarname)
	if not self.CVars[cvarname] then
		return nil
	end
	return self.CVars[cvarname].value
end

function CVarManager:SaveCVars()
	local file = jar.fs.OpenWrite(self.configfile)
	if not file then
		jar.Logger.GetDefaultLogger():Error("Could not write file " .. self.configfile .."! " .. jar.fs.GetLastError())
		return false
	end
	file:WriteString("-- automatically generated CVar list. Changes to the layout and comments will be discarded on next save.\n")
	
	--ordering by name
	local saveme = {}
	for _, cvar in pairs(self.CVars) do
		if cvar.flags.save and cvar.value ~= cvar.defaultValue then
			table.insert(saveme, {name = cvar.name, code = cvar.name .. " = " .. cvar:ToLua() .. "\n" })
		end
	end
	table.sort(saveme, function(first, second) return first.name < second.name end)
	for _, info in ipairs(saveme) do
		file:WriteString(info.code)
	end
	file:Close()
	return true
end

function CVarManager:LoadCVars()
	local code, err = loadfile(self.configfile)
	if not code then
		-- it's probably not that bad if this couldn't be opened (that probably means it doesn't exist yet, maybe because the user deleted it. or it may contain errors, in which case it may be overwritten later, which could enrage the user.)
		jar.Logger.GetDefaultLogger():Info("Could not load config file " .. self.configfile ..": " .. err, 0)
		return false
	end
	-- create secure function environment
	local settings = {}
	setfenv(code, settings)
	-- try and execute file
	local success, err = pcall(code)
	if not success then
		-- caught an error
		jar.Logger.GetDefaultLogger():Warning(self.configfile.." is not applied due to an error: " .. err)
		return false
	end
	-- for each setting:
	for name, val in pairs(settings) do
		self:SetCVar(name, val)
	end
	return true
end


-- looks for cvars that start with cvarname. returns cvarname_long, possibilities where possibilities is the list of all cvarnames that start with cvarname and cvarname_long is the longest possible string that would return the same possibilities, or the original string if there are none.
function CVarManager:AutoComplete(cvarname)
	local available = {}
	for name, _ in pairs(self.CVars) do
		table.insert(available, name)
	end
	table.sort(available)
	return AutoComplete(cvarname, available, false)
end