--[[!Console Commands
--]]

CCommand = {}


-- special case: name starts with +.
-- this indicates that this can be bound to axes, and the first (and only!) parameter is the amount.
-- a name starting with a plus indicates that the first parameter should be the amount (1 on KeyPress, 0 on KeyRelease, real number between 0 and 1 on Axis)
CCommand.name = ""
CCommand.description = ""
CCommand.Execute = function(self) end

function CCommand:New(info)
	local obj = {}
	setmetatable(obj, self)
	self.__index = self
	
	-- info has to be sanitized.
	
	-- this is always a string, unless the user bypasses the CCommandManager, which he shouldn't do..
	if type(info.name) ~= "string" then
		error("CCommand name is not a string! Please use the CCommandManager to create CVars!", 2)
	end
	obj.name = info.name
	
	local function sanitize(key, desiredType, default)
		if info[key] then
			if type(info[key]) ~= desiredType then
				jar.Logger.GetDefaultLogger():Warning("CVar:New(): CVar " .. info.name ..": info."..key.." has invalid type " .. type(obj[key]) .. " (desired: " .. desiredType .. "), ignoring!")
				obj[key] = default
			else
				obj[key] = info[key]
			end
		else
			obj[key] = default
		end
	end
	sanitize("description", "string", "[no description]")
	sanitize("Execute", "function", function() end)
	
	return obj
end