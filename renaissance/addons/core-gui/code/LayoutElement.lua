require("Expression.lua")

LayoutElement =
{
	-- array of available LayoutElement - each element class should register itself via LayoutElement.Register(element)
	available = {},
	
	-- to be set by derived classes - name used in Layout definition files
	classname = nil,
	
	-- Expressions for calculating position and size based on given width and height values
	-- filled by FromTable
	position_expression = nil,
	size_expression = nil,
	
	-- If this LayoutElement contains any other LayoutElements, they go here (array)
	childElements = nil,
	
	-- All the UIControls (Buttons etc.) go here (array)
	controls = nil,
}

-- Whether a given object is a LayoutElement (or derived from it)
function IsLayoutElement(obj)
	-- walk down the hierarchy and see if LayoutElement is the metatable.__index at some point
	while true do
		local mt = getmetatable(obj)
		if not mt then
			return false
		end
		obj = mt.__index
		if not obj then
			return false
		elseif obj == LayoutElement then
			return true
		end
	end
end

-- Makes a LayoutElement available in Layout definition files (it needs to have classname set)
-- (static function)
function LayoutElement.Register(element)
	if not IsLayoutElement(element) then error("Trying to register something that's no LayoutElement as one!") end
	if not element.classname then error("No classname set!") end
	table.insert(LayoutElement.available, element)
end

-- Adds the functions for the registered layout elements to the supplied function environment
-- (static function)
function LayoutElement.AddAvailableToFEnv(fenv)
	for _, class in ipairs(LayoutElement.available) do
		fenv[class.classname] = function(t)
			local object = class:New()
			object:FromTable(t)
			return object
		end
	end
end

-- Creates a new LayoutElement object (or derived class, usually)
function LayoutElement:New()
	jar.Logger.GetDefaultLogger():Info("Creating a " .. (self.classname or "???") .. " LayoutElement!", 3) --TODO: DELETEME
	self.__index = self
	local obj = setmetatable({}, self)
	return obj
end

-- sets the properties from a given table
-- raises an error when something crucial is missing
-- to be further specialised by derived classes
function LayoutElement:FromTable(t)
	-- name
	if type(t.name) ~= "string" then error("LayoutElement has no name string!") end
	self.name = t.name
	
	-- position, size
	local function CheckAndAddVector(name)
		if type(t[name]) ~= "table" then
			error("LayoutElement has no valid " .. name .. " set!")
		end
		local expX = t[name][1]
		if type(expX) == "number" then
			expX = Expression:New(Expression.Type.Constant, expX)
		elseif not IsExpression(expX) then
			error("LayoutElement has no valid " .. name .. " set!")
		end
		local expY = t[name][2]
		if type(expY) == "number" then
			expY = Expression:New(Expression.Type.Constant, expY)
		elseif not IsExpression(expY) then
			error("LayoutElement has no valid " .. name .. " set!")
		end
		self[name .. "_expression"] = { expX, expY }
	end
	CheckAndAddVector("position")
	CheckAndAddVector("size")
	
	-- child Elements
	self.childElements = {}
	for _, obj in pairs(t) do
		if IsLayoutElement(obj) then
			table.insert(self.childElements, obj)
		end
	end
end

-- Recursively gets all child elements, i.e. their child elements too.
function LayoutElement:GetChildElements(t)
	t = t or {}
	for _, child in ipairs(self, self.childElements) do
		table.insert(t, child)
		child:GetChildElements(t)
	end
	return t
end

-- Called whenever the parent's size changes
-- To be further specialized by derived classes to reposition the Controls
function LayoutElement:ChangeSize(width, height)
	local vars = { width = width, height = height }
	self.position = jar.Vector2f(self.position_expression[1]:Evaluate(vars), self.position_expression[2]:Evaluate(vars))
	local newSize = jar.Vector2f(self.size_expression[1]:Evaluate(vars), self.size_expression[2]:Evaluate(vars))
	if newSize ~= self.size then
		self.size = newSize
		for _, child in ipairs(self.childElements) do
			child:ChangeSize(newSize.X, newSize.Y)
		end
	end
end

-- Renders this element to the given rendertarget
function LayoutElement:Draw(target, offset)
	if not self.position or not self.size then error("Layout Element's size not initialized (call ChangeSize())!") end
	offset = offset or jar.Vector2f(0, 0)
	offset = offset + self.position
	
	local function DrawChildren()
		if not self.childElements then return end
		for _, child in ipairs(self.childElements) do
			child:Draw(target, offset)
		end
	end
	local function DrawControls()
		if not self.controls then return end
		for _, control in pairs(self.controls) do
			control:Draw(target)
		end
	end
	
	-- change view so 0, 0 is this element's absolute position
	local view = target:GetView()
	view:SetCenter(self.size / 2)
	view:SetSize(self.size)
	
	local targetSize = target:GetSize()
	local viewport = view:GetViewport()
	viewport.left = offset.X / targetSize.X
	viewport.top = offset.Y / targetSize.Y
	viewport.width = self.size.X / targetSize.X
	viewport.height = self.size.Y / targetSize.Y
	
	view:SetViewport(viewport)
	target:SetView(view)
	
	--TODO: DELETEME (viewport test code)
	testRect:SetFillColor(color)
	target:Draw(testRect)
	color.R = color.R + 64
	color.G = color.G - 64
	
	DrawChildren()
	DrawControls()
end