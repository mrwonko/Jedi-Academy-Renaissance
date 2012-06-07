MenuControl =
{
	-- array of available MenuControl - each control class should register itself via MenuControl.Register(control)
	available = {},
	
	-- to be set by derived classes - name used in Menu definition files
	classname = nil,
	
	-- to be set by deriving classes in FromTable()
	size = jar.Vector2f(0, 0),
}
local MenuControl = MenuControl

-- Whether a given object is a MenuControl (or derived from it)
function IsMenuControl(obj)
	-- walk down the hierarchy and see if MenuControl is the metatable.__index at some point
	while true do
		local mt = getmetatable(obj)
		if not mt then
			return false
		end
		obj = mt.__index
		if not obj then
			return false
		elseif obj == MenuControl then
			return true
		end
	end
end

-- Makes a MenuControl available in Menu definition files (it needs to have classname set)
-- (static function)
function MenuControl.Register(control)
	if not IsMenuControl(control) then error("Trying to register something that's no MenuControl as one!") end
	if not control.classname then error("No classname set!") end
	table.insert(MenuControl.available, control)
end

-- Adds the functions for the registered controls to the supplied function environment
-- (static function)
function MenuControl.AddAvailableToFEnv(fenv)
	for _, class in ipairs(MenuControl.available) do
		fenv[class.classname] = function(t)
			local object = class:New()
			object:FromTable(t)
			return object
		end
	end
end

-- Creates a new MenuControl object (or derived class, usually)
function MenuControl:New()
	jar.Logger.GetDefaultLogger():Info("Creating a " .. (self.classname or "???") .. " MenuControl!", 3) --TODO: DELETEME
	self.__index = self
	local obj = setmetatable({}, self)
	return obj
end

-- sets the properties from a given table
-- raises an error when something crucial is missing
-- to be further specialised by derived classes
function MenuControl:FromTable(t)
	if t.text and type(t.text) ~= "string" then error("MenuControl text is no string!") end
	self.text = t.text
	if type(t.order) ~= "number" then error("MenuControl has no order number!") end
	if type(t.container) ~= "string" then error("MenuControl has no container string!") end
	self.order = t.order
	self.container = t.container
end

-- Renders this control to the given rendertarget
-- Where to draw is determined by SetPosition(), size should be limited by what GetSize() returns
-- to be implemented by derived classes
function MenuControl:Draw(target)
end

-- To be implemented by derived classes
function MenuControl:SetPosition(pos)
end