require("LayoutElement.lua")

-- inherits from Drawable2D, i.e. can be drawn to a Window
Layout = {}
local Layout = Layout

function Layout:New()
	local obj = setmetatable({}, self)
	self.__index = self
	return obj
end

-- Create a new instance of a given layout
function Layout:NewInstance()
	assert(self ~= Layout)
	assert(type(self.elementsByName) == "table")
	local obj = setmetatable({}, {__index = self})
	
	local elementsByName = self.elementsByName
	local newElementsByName = {}
	obj.elementsByName = newElementsByName
	for name, element in pairs(elementsByName) do
		element = element:New()
		newElementsByName[name] = element
		element.controls = {}
	end
	
	return obj
end

function Layout:Draw(target)
	if not self.elementsByName then error("Trying to draw uninitialized Layout (call FromTable() first)!") end
	local prevView = target:GetView()
	for _, element in pairs(self.elementsByName) do
		element:Draw(target)
	end
	target:SetView(prevView)
end

-- sets the properties from a given table
-- raises an error when something crucial is missing
function Layout:FromTable(t)
	-- name
	if type(t.name) ~= "string" then error("No name string supplied!", 3) end
	self.name = t.name
	
	-- LayoutElements
	local elements = {}
	self.elementsByName = elements
	-- this is only possibly because the layout element functions raise an error instead of returning nil when they fail - otherwise there'd be gaps in the array.
	for _, obj in ipairs(t) do
		if IsLayoutElement(obj) then
			if elements[obj.name] then
				jar.Logger.GetDefaultLogger():Warning("Duplicate LayoutElement \"" .. obj.name .. "\" in Layout \"" .. self.name .. "\"! Discarding latter.")
			end
			elements[obj.name] = obj
		end
	end
end

function Layout:ChangeSize(width, height)
	if not self.elementsByName then error("Trying to change size of uninitialized Layout (call FromTable() first)!") end
	for _, obj in pairs(self.elementsByName) do
		obj:ChangeSize(width, height)
	end
end