require("InstructionInterpreter.lua")
require("CCommandManager.lua")

print("==== Test Suite Init ====")
print("")
local oldLogLevel = g_CVarManager:GetCVar("log_level")
g_InstructionInterpreter:Interpret("log_level; echo a    little test; echo \"a little   fucking test\"; log_level = 42; log_level; log_level = " .. oldLogLevel .. "; log_level;")
g_InstructionInterpreter:Interpret("help log_level")
g_InstructionInterpreter:Interpret("help stupidNonexistantShit")

print("==== Test Suite Start ====")

print ""
--[[
local str, array = g_CVarManager:AutoComplete("f")
print("/f")
print("/" .. str)
for _, item in ipairs(array) do
	print("  " .. item)
end
--]]

local aLittleCircle = jar.Shape.Circle(0, 0, 32, jar.Color(0, 0, 0, 0), 1, jar.Color.Red)
aLittleCircle:SetY(300)

local moar = true
local x = 400
aLittleCircle:SetX(x)

-- coroutine testing
dofile("Code/coroutine_tests.lua")

local running = true
while running do --TODO: delete and false
	while true do
		local success, event = jar.EventManager.GetSingleton():GetEvent()
		if not success then
			-- no more events available
			break
		end
		
		if event.Type == jar.Event.Closed then
			running = false
		elseif event.Type == jar.Event.KeyPressed and event.Key.Code == jar.Key.Escape then
			running = false
		end
	end
	
	local frametime = g_testWindow:GetFrameTime()
	if frametime == 0 then -- I clip the frame rate to a thousand fps because my time is in milliseconds.
		jar.Sleep(1)
		frametime = g_testWindow:GetFrameTime()
	end
	
	jar.Core.GetSingleton():Update(frametime)
	
	aLittleCircle:SetX(x)
	
	if moar then
		x = x + 0.1 * frametime
	else
		x = x - 0.1 * frametime
	end
	
	if x <= 50 or x >= 750 then
		moar = not moar
	end
	
	g_testWindow:Clear(jar.Color.Black)
	g_testWindow:Draw(aLittleCircle)
	g_testWindow:Display()
end

--g_CVarManager:SaveCVars()
print("==== Test Suite End ====")