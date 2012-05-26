require("LayoutElement.lua")

-- inherits from Drawable2D, i.e. can be drawn to a Window
class "Layout" (jar.Drawable2D)

-- to have the same interface as my Lua classes
function Layout.New()
	return Layout()
end

-- Constructor
function Layout:__init()
	jar.Drawable2D.__init(self)
end


function Layout:Draw(target, states)
	if not self.elements then error("Trying to draw uninitialized Layout (call FromTable() first)!") end
	local prevView = target:GetView()
	for _, element in ipairs(self.elements) do
		element:Draw(target)
	end
	target:SetView(prevView)
end

-- sets the properties from a given table
-- raises an error when something crucial is missing
function Layout:FromTable(t)
	-- name
	if type(t.name) ~= "string" then error("No name string supplied!") end
	self.name = t.name
	
	-- LayoutElements
	self.elements = {}
	-- this is only possibly because the layout element functions raise an error instead of returning nil when they fail - otherwise there'd be gaps in the array.
	for _, obj in ipairs(t) do
		if IsLayoutElement(obj) then
			table.insert(self.elements, obj)
		end
	end
end

function Layout:ChangeSize(width, height)
	if not self.elements then error("Trying to change size of uninitialized Layout (call FromTable() first)!") end
	for _, obj in ipairs(self.elements) do
		obj:ChangeSize(width, height)
	end
end