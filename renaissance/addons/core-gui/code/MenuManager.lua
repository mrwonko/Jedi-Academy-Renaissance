-- == Menu Manager ==
--  keeps track of existing menus and the current menu stack

require("Menu.lua")
require("MenuControl.lua")

-- common
local menuFEnv = setmetatable({}, {__index = _G})
-- add Layout Element functions
MenuControl.AddAvailableToFEnv(menuFEnv)


MenuManager = {}
local MenuManager = MenuManager -- for faster access (locals are registers)
local GetDefaultLogger = jar.Logger.GetDefaultLogger

function MenuManager:New(layoutManager)
	if not layoutManager then error("No layoutManager supplied in MenuManager:New()!") end
	local obj =
	{
		menus = {}, -- available menus by name
		menuStack = {size = 0}, -- open menus
	}
	self.__index = self
	self.layoutManager = layoutManager
	setmetatable(obj, self)
	return obj
end

function MenuManager:ParseFile(filename)
	local f, err = loadfile(filename)
	if not f then
		GetDefaultLogger():Error("Could not load menu file \"" .. filename .. "\":\n" .. err )
		return false
	end
	
	-- function environment containing new, menu related functions and variables (width, height) to use
	local fEnv = setmetatable({}, {__index = menuFEnv})
	local menus = self.menus -- for faster access
	local layoutManager = self.layoutManager
	
	-- add menu function
	function fEnv.Menu(t)
		if type(t) ~= "table" then error("No table supplied to Menu()!", 2) end
		local menu = Menu()
		menu.filename = filename
		menu:FromTable(t, layoutManager)
		-- is there already a menu of this name?
		if menus[menu.name] then
			-- warning!
			GetDefaultLogger():Warning("Duplicate Menu \"" .. menu.name .. "\" in \"" .. filename .. "\"! Ignoring. (Previous one was in \"" .. menus[menu.name].filename .. "\")")
		else
			-- add new menu to menu map
			menus[menu.name] = menu
			GetDefaultLogger():Info("Adding Menu \"" .. menu.name .. "\"", 3)
		end
	end
	
	setfenv(f, fEnv)
	
	local success, err = xpcall(f, AddTracebackToError)
	if not success then
		GetDefaultLogger():Error("Error parsing menu file \"" .. filename .. "\":\n" .. err)
		return false
	end
end

function MenuManager:ParseMenus()
	for _, filename in ipairs(jar.fs.GetFilesInDirectory("code/ui/menus")) do
		self:ParseFile("code/ui/menus/" .. filename)
	end
end

function MenuManager:SetScreenSize(width, height)
	for _, menu in pairs(self.menus) do
		menu:ChangeSize(width, height)
	end
end

function MenuManager:OpenMenu(name)
	local menu = self.menus[name]
	if not menu then error("No menu \"" .. name .. "\" available!", 2) end
	local stack = self.menuStack
	stack.size = stack.size + 1
	stack[stack.size] = menu
end

function MenuManager:CloseMenu()
	local stack = self.menuStack
	stack[stack.size] = nil
	stack.size = stack.size - 1
end

function MenuManager:CloseAllMenus()
	local stack = self.menuStack
	for i = 1, stack.size do
		stack[i] = nil
	end
	stack.size = 0
end

function MenuManager:IsMenuOpen()
	return self.menuStack.size > 0
end

function MenuManager:GetCurrentMenu()
	local stack = self.menuStack
	return stack[stack.size]
end

function MenuManager:DrawCurrentMenu(target)
	local stack = self.menuStack
	local menu = stack[stack.size]
	if menu then
		target:Draw(menu)
	end
end