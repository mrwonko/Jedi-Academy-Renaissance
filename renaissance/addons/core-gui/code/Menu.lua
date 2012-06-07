require("MenuControl.lua")
require("LayoutManager.lua")

-- inherits from Drawable2D, i.e. can be drawn to a Window
class "Menu" (jar.Drawable2D)
local Menu = Menu

-- Constructor
function Menu:__init()
	jar.Drawable2D.__init(self)
end

function Menu:New()
	assert(self == nil or self == Menu)
	return Menu()
end

function Menu:Draw(target, states)
	if not self.layout then error("Trying to draw uninitialized Menu (call FromTable() first)!") end
	--target:Draw(self.layout)
	self.layout:Draw(target)
end

-- sets the properties from a given table
-- raises an error when something crucial is missing
function Menu:FromTable(t, layoutManager)
	-- name
	if type(t.name) ~= "string" then error("No menu name string supplied!", 3) end
	if type(t.layout) ~= "string" then error("No menu layout string supplied!", 3) end
	self.name = t.name
	layout = layoutManager.layouts[t.layout]
	if not layout then error("No such menu layout!", 3) end
	
	-- create new layout instance
	layout = layout:NewInstance()
	self.layout = layout
	local elementsByName = layout.elementsByName
	
	-- this is only possibly because the menu control functions raise an error instead of returning nil when they fail - otherwise there'd be gaps in the array.
	for _, obj in ipairs(t) do
		if IsMenuControl(obj) then
			local element = elementsByName[obj.container]
			if not element then error("No LayoutElement \"" .. obj.container .. "\" in Layout "\" .. layout.name .. "\"!", 3)  end
			table.insert(element.controls, obj)
		end
	end
end

function Menu:ChangeSize(width, height)
	self.layout:ChangeSize(width, height)
end