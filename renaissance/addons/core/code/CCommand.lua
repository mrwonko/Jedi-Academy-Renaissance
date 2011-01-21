--[[!Console Commands
--]]

CCommand =
{
	-- a name starting with a plus indicates that OnStop should be called when the button is no longer pressed.
	name = "",
	
	description = "",
	
	OnStart = function(self) end,
	
	--OnStop only gets called for commands starting with a plus
	OnStop = function(self) end,
}

function CCommand:New(info)
	local obj = info or {}
	
	setmetatable(obj, self)
	self.__index = self
	
	-- info has to be sanitized.
	
	-- this is always a string, unless the user bypasses the CCommandManager, which he shouldn't do..
	if type(obj.name) ~= "string" then
		error("CCommand name is not a string! Please use the CCommandManager to create CVars!", 2)
	end
	local function sanitize(key, desiredType, default)
		if obj[key] then
			if type(obj[key]) ~= desiredType then
				jar.Logger.GetDefaultLogger():Warning("CVar:New(): CVar " .. obj.name ..": info."..key.." has invalid type " .. type(obj[key]) .. " (desired: " .. desiredType .. "), ignoring!")
				obj[key] = default
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