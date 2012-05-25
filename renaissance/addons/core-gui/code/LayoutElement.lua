LayoutElement =
{
	-- array of available LayoutElement - each element class should register itself via LayoutElement.Register(element)
	available = {},
	-- to be set by derived classes
	classname = nil,
}

function LayoutElement.Register(element)
	if not element.classname then error("No classname set!") end
	table.insert(LayoutElement.available, element)
end

function LayoutElement:New()
	self.__index = self
	local obj = setmetatable({}, self)
	return obj
end

-- to be further specialised by derived classes
-- sets the properties from a given table
-- raises an error when something crucial is missing
function LayoutElement:FromTable(t)
	-- todo
end

-- Adds the functions for the registered layout elements to the supplied function environment
-- (static function)
function LayoutElement.AddAvailableToFEnv(fenv)
	for _, class in ipairs(LayoutElement.available) do
		fenv[class.classname] = function(t)
			local object = class:New()
			if object:FromTable(t) then
				return object
			end
		end
	end
end