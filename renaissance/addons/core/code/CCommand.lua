--[[!Console Commands
--]]

CCommand =
{
	-- a name starting with a plus indicates that OnStop should be called when the button is no longer pressed.
	name = "",
	
	description = "",
	
	OnStart = function() end,
	
	--OnStop only gets called for commands starting with a plus
	OnStop = function() end,
}

function CCommand:New(info)
	local obj = {}
	
	setmetatable(obj, self)
	self.__index = self
	
	-- info has to be sanitized.
	obj.name = info.name -- this is always a string, unless the user bypasses the CCommandManager, which he shouldn't do..
	if type(info.name) ~= "string" then
		error("CCommand name is not a string! Please use the CVarManager to create CVars!", 2)
	end
	local function sanitize(key, desiredType, default)
		if info[key] then
			if type(info[key]) == desiredType then
				obj[key] = info[key]
			else
				jar.Logger.GetDefaultLogger():Warning("CVar:New(): CVar " .. info.name ..": info."..key.." has invalid type " .. type(info[key]) .. " (desired: " .. desiredType .. "), ignoring!")
			end
		else
			obj[key] = default
		end
	end
	sanitize("description", "string", "[no description]")
	sanitize("OnStart", "function", function() end)
	sanitize("OnStop", "function", function() end)
	return obj
end