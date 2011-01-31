require("InstructionInterpreter.lua")
require("CCommandManager.lua")

print("==== Test Suite Start ====")
print("")
print("==== InstructionInterpreter tests ====")

local oldLogLevel = g_CVarManager:GetCVar("log_level")
g_InstructionInterpreter:Interpret("log_level; echo a    little test; echo \"a little   fucking test\"; log_level = 42; log_level; log_level = " .. oldLogLevel .. "; log_level;")
g_InstructionInterpreter:Interpret("help log_level")
g_InstructionInterpreter:Interpret("help stupidNonexistantShit")
g_InstructionInterpreter:Interpret("stupidNonexistantShit")

g_InstructionInterpreter:Interpret("echo \"pre-wait\"; wait 12; echo \"post-wait\"; wait 8; echo lol")
g_InstructionInterpreter:Update(10)
g_InstructionInterpreter:Update(10)

print ""

local testImage = jar.Image()
if not testImage:LoadFromFile("textures/tests/me2.jpg") then
	error("Could not load test image!")
end
local testSprite = jar.Sprite(testImage)

local testFont = jar.Font()
if not testFont:LoadFromFile("fonts/anewhope") and not testFont:LoadFromFile("fonts/arial") then -- I don't distribute anewhope since it's a jka rip.
	error("Could not load font!")
end

local testText = jar.Text(testFont)
testText:SetText("Ich bin der schöne Testtext!\nUnd ich erstrecke mich über mehrere Zeilen.\n(Sorry for that text being German,\nI wanted to test ö, ß etc.)")
testText:SetPosition(0, 400)
print("Original size: " .. testText:GetFontSize() .. "pt")
testText:SetFontSize(8)

local moar = true
local x = 400
local aLittleCircle = jar.Shape.Circle(0, 0, 32, jar.Color(0, 0, 0, 0), 1, jar.Color.Red)
aLittleCircle:SetY(300)
aLittleCircle:SetX(x)

local running = true
while running do
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
	--g_testWindow:Draw(testRect)
	--g_testWindow:Draw(testText)
	g_testWindow:Draw(testSprite)
	g_testWindow:Draw(testText)
	g_testWindow:Display()
end

--g_CVarManager:SaveCVars()
print("==== Test Suite End ====")