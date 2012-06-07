require("Expression.lua")
require("Layout.lua")
require("LayoutElement.lua")

-- common
local layoutFEnv =
{
	width = Expression:New(Expression.Type.Variable, "width"),
	height = Expression:New(Expression.Type.Variable, "height"),
}
-- add Layout Element functions
LayoutElement.AddAvailableToFEnv(layoutFEnv)


LayoutManager = {}
local LayoutManager = LayoutManager

function LayoutManager:New()
	local obj =
	{
		layouts = {}, -- available layouts by name
	}
	self.__index = self
	setmetatable(obj, self)
	return obj
end

function LayoutManager:ParseFile(filename)
	local f, err = loadfile(filename)
	if not f then
		jar.Logger.GetDefaultLogger():Error("Could not load layout file \"" .. filename .. "\":\n" .. err )
		return false
	end
	
	-- function environment containing new, layout related functions and variables (width, height) to use
	local fEnv = setmetatable({}, {__index = layoutFEnv})
	
	-- add Layout function
	function fEnv.Layout(t)
		if type(t) ~= "table" then error("No table supplied to Layout()!", 2) end
		local layout = Layout:New()
		layout.filename = filename
		layout:FromTable(t)
		-- is there already a layout of this name?
		if self.layouts[layout.name] then
			-- warning!
			jar.Logger.GetDefaultLogger():Warning("Duplicate Menu Layout \"" .. layout.name .. "\" in \"" .. filename .. "\"! Ignoring. (Previous one was in \"" .. self.layouts[layout.name].filename .. "\")")
		else
			-- add new layout to layout map
			self.layouts[layout.name] = layout
		end
	end
	
	setfenv(f, fEnv)
	
	local success, err = xpcall(f, AddTracebackToError)
	if not success then
		jar.Logger.GetDefaultLogger():Error("Error parsing layout file \"" .. filename .. "\":\n" .. err)
		return false
	end
end

function LayoutManager:ParseLayouts()
	for _, filename in ipairs(jar.fs.GetFilesInDirectory("code/ui/layouts")) do
		self:ParseFile("code/ui/layouts/" .. filename)
	end
end

function LayoutManager.GetWidthVar()
	return layoutFEnv.width
end

function LayoutManager.GetHeightVar()
	return layoutFEnv.height
end