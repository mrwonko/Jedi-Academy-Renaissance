--[[! console variables.
--]]

CVar =
{
	-- the values 
	name = "",
	
	type = 0,
	-- type depends on type (d'oh!)
	value = "",
	defaultValue = "",
	
	description = "",
	
	-- flags and their default values
	flags =
	{
		-- Whether this was entered by a user in the console/config, but not (yet?) registered in code.
		-- Biggest differenceis that no flags are set that way and that it lacks a description.
		userCreated = false,
		-- whether this should be saved to the config
		save = true,
		-- whether this cannot be changed via console (but still via code!)
		readonly = false,
	},
	
	-- changing the below in your info doesn't make sense.
	
	--type enum
	TYPES =
	{
		ANY = -1, -- will I ever use this?
		STRING = 0,
		INT = 1,
		FLOAT = 2,
		BOOLEAN = 3,
	},
	
	typeLookup =
	{
		[0] = "string",
		[1] = "number",
		[2] = "number",
		[3] = "boolean",
	},
}

--[[ Creates a new CVar. You'll usually not call this directly but use CVarManager:RegisterCVar(). info can contain an OnChange(newVal) function that is called whenever a cvar has been changed, as well as an IsValid(newVal) function do verify validity of a new value.
--]]
function CVar:New(info)
	local obj = {}
	
	setmetatable(obj, self)
	self.__index = self
	
	-- info has to be sanitized.
	obj.name = info.name -- this is always a string, unless the user bypasses the CVarManager, which he shouldn't do..
	assert(type(info.name) == "string")
	
	local function sanitize(key, desiredType)
		if info[key] then
			if type(info[key]) == desiredType then
				obj[key] = info[key]
			else
				jar.Logger.GetDefaultLogger():Warning("CVar:New(): CVar " .. info.name ..": info."..key.." has invalid type " .. type(info[key]) .. " (desired: " .. desiredType .. "), ignoring!")
			end
		end
	end
	sanitize("type", "number")
	if obj.type == self.TYPES.ANY and info.defaultValue ~= nil then
		obj.defaultValue = info.defaultValue
	else
		sanitize("defaultValue", self.typeLookup[obj.type])
	end
	sanitize("description", "string")
	sanitize("OnChange", "function")
	sanitize("IsValid", "function")
	
	local function sanitizeFlag(flag)
		if info.flags[flag] then
			if type(info.flags[flag]) == "boolean" then
				obj.flags[flag] = info.flags[flag]
			else
				jar.Logger.GetDefaultLogger():Warning("CVar:New(): CVar " .. info.name ..": info.flags."..flag.." has invalid type " .. type(info.flag[key]) .. " (desired: boolean), ignoring!")
			end
		end
	end
	if type(info.flags) == "table" then
		sanitizeFlag("save")
		sanitizeFlag("readonly")
	end
	
	-- make sure OnChange gets called, i.e. use SetValue for value. SetValue() takes care of validating the type.
	if info.value then
		if info.value ~= obj.defaultValue then
			obj:SetValue(info.value)
		else
			obj.value = info.value
		end
	end
	
	return obj
end

function CVar:SetValue(newVal)
	-- check newVal's validity
	-- is it of the right type, as defined in self.type?
	if self.type ~= CVar.TYPES.ANY and type(newVal) ~= CVar.typeLookup[self.type] then
		jar.Logger.GetDefaultLogger():Warning("Value supplied for CVar " .. self.name .. " has invalid type " .. type(newVal) .. "!")
		return false
	elseif self.minValue and newVal < self.minValue then
		jar.Logger.GetDefaultLogger():Warning("CVar " .. self.name .. " must be at least " .. self.minValue .."!")
		return false
	elseif self.maxValue and newVal > self.maxValue then
		jar.Logger.GetDefaultLogger():Warning("CVar " .. self.name .. " must be no greater than " .. self.maxValue .. "!")
		return false
	-- has the user defined a custom IsValid function?
	elseif type(self.IsValid) == "function" then
		if not self.IsValid(newVal) then
			return false
		end
	end
	-- ints need to be floored
	if self.type == CVar.TYPES.INT then
		newVal = math.floor(newVal)
	end
	-- it's valid. we can set it.
	if type(self.OnChange) == "function" then
		self.OnChange(self.value, newVal)
	end
	self.value = newVal
	return true
end

function CVar:Print(printFunc)
	printFunc = printFunc or print
	
	local printMe = self.name .. " = " .. self:ToString() .. " (default: " .. (tostring(self.defaultValue) or "[can't display]") .. ")"
	if self.flags.readonly then
		printMe = printMe .. " [readonly]"
	end
	if self.minValue and self.maxValue then
		printMe = printMe .. "\n" .. (tostring(self.minValue) or "[can't display]") .. " <= " .. self.name .. " <= " .. (tostring(self.maxValue) or "[can't display]")
	elseif self.minValue then
		printMe = printMe .. "\n" .. (tostring(self.minValue) or "[can't display]") .. " <= " .. self.name
	elseif self.maxValue then
		printMe = printMe .. "\n" .. self.name .. " <= " .. (tostring(self.maxValue) or "[can't display]")
	end
	if self.description then
		printMe = printMe .. "\n" .. self.description
	end
	printFunc(printMe)
end

function CVar:ToString()
	if self.type == self.TYPES.STRING then
		return self.value
	end
	return tostring(self.value) or ""
end

function CVar:ToFloat()
	return tonumber(self.value) or 0
end

function CVar:ToInt()
	return math.floor(tonumber(self.value) or 0)
end

function CVar:ToBool()
	if self.value and self.value ~= 0 and self.value ~= "" then
		return true
	else
		return false
	end
end

function CVar:ToType(t)
	if t == CVar.TYPES.ANY then
		return self.value
	elseif t == CVar.TYPES.STRING then
		return self:ToString()
	elseif t == CVar.TYPES.INT then
		return self:ToInt()
	elseif t == CVar.TYPES.FLOAT then
		return self:ToFloat()
	elseif t == CVar.TYPES.BOOLEAN then
		return self:ToBool()
	else
		jar.Logger.GetDefaultLogger():Warning("CVar:ToType() called with invalid type" .. ( (type(t) == "number" and " "..t) or "") .."!")
		return nil
	end
end

function CVar:ToLua()
	local t = type(self.value)
	if t == "string" then
		-- TODO: Escape " and \n etc. in self.value!
		jar.Logger.GetDefaultLogger():Warning("CVar:ToLua(): Unprotected against code injection / errors!")
		return "\"" .. self.value .. "\""
	elseif t == "number" then
		return self.value
	elseif t == "boolean" then
		return (self.value and "true") or "false"
	else
		return "nil --[[ unhandled cvar type " .. t .. " ]]"
	end
end

-- for array-strings (values separated by delim(iter), usually semicolon (;)
function CVar:ToArray(delim)
	delim = delim or ";"
	local str = self:ToString()
	local output = {}
	while #str > 0 do
		local delimPos = string.find(str, delim)
		if delimPos then
			table.insert(output, string.sub(str, 1, delimPos-1))
			str = string.sub(str, delimPos + #delim)
		else
			table.insert(output, str)
			break
		end
	end
	return output
end