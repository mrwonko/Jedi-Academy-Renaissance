-- Test expressions

require("Expression.lua")

print("\n= Testing Expressions =\n")

local two = Expression:New(Expression.Type.Constant, 2)
local test = 4 + two * 5
print("4 + 2 * 5 = " .. test:Evaluate())

local var = Expression:New(Expression.Type.Variable, "var")
local test2 = 2 * 4 + var * 5
for _, i in ipairs{1, 2, 5} do
	vars = {var = i}
	print("4 * 2 + " .. i .. " * 5 = " .. test2:ToString(vars) .. " = " .. test2:Evaluate(vars))
end


-- Test layouts/menus
print("\n= Testing Menus =\n")

require("LayoutManager.lua")
require("MenuManager.lua")

g_layoutManager = LayoutManager:New()
local g_layoutManager = g_layoutManager
g_layoutManager:ParseLayouts()

g_menuManager = MenuManager:New(g_layoutManager)
local g_menuManager = g_menuManager
g_menuManager:ParseMenus()
g_menuManager:SetScreenSize(800, 600)
g_menuManager:OpenMenu("main")

local window = jar.RenderWindow(800, 600, "Test Window", true, false) -- border, fullscreen

local lastFrametime = jar.GetTime()
local function CalcDeltaT()
	local frametime = jar.GetTime()
	assert(frametime >= lastFrametime)
	if frametime == lastFrametime then -- I clamp the frame rate to a thousand fps because my time is in milliseconds.
		jar.Sleep(1)
		frametime = jar.GetTime()
	end
	local deltaT = math.min(frametime - lastFrametime, 100) -- no longer than 100ms per frame (e.g. breakpoints in debugging might screw up timings otherwise)
	lastFrametime = frametime
	return deltaT
end

testRect = jar.RectangleShape(jar.Vector2f(800, 600))

local running = true
while running do
	local deltaT = CalcDeltaT()
	jar.Core.GetSingleton():Update(deltaT) -- won't get events otherwise
	
	while true do
		local success, event = jar.EventManager.GetSingleton():GetEvent()
		if not success then
			-- no more events available
			break
		end
		
		if event.Type == jar.Event.Closed or (event.Type == jar.Event.KeyPressed and event.Key.Code == jar.Key.Escape) then
			running = false
		end
	end
	
	color = jar.Color(0, 255, 0) --DELETEME : test
	
	window:Clear(jar.Color.Black)
	if true then
		g_menuManager:DrawCurrentMenu(window)
	elseif true then
	window:Draw(menu)
	else
	menu:Draw(window)
	end
	window:Display()
end