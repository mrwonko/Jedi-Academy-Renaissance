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

local aLittleCircle = jar.Shape.Circle(0, 0, 32, jar.Color(0, 0, 0, 0), 1, jar.Color.Red)
aLittleCircle:SetY(300)

--[[
local testText = jar.String("Test yes!\nTest yes!")
testText:SetSize(12)
testText:SetPosition(1,  1)
testText:SetColor(jar.Color.White)

local function GetCharWidth(font, charnum)
	return font:GetGlyph(charnum).Rectangle:GetWidth() / font:GetCharacterSize()
end

local font = testText:GetFont()
for i = 0, #testText:GetText() - 1 do
	print("Char " .. i .. " ("..testText:GetText():sub(i+1, i+1)..") is " .. testText:GetSize() * GetCharWidth(font, testText:GetText():byte(i+1)).." wide")
end

local function CharPosPrint(str, index)
	local pos = str:GetCharacterPos(index)
	print("Character " .. index .. " is at (" .. pos.x .. ", " .. pos.y ..")")
end
CharPosPrint(testText, 0)
CharPosPrint(testText, 1)
CharPosPrint(testText, 8)
CharPosPrint(testText, 9)
CharPosPrint(testText, 10)
--print("Height: " .. testText:GetRect():GetHeight())
--local testRect = jar.Shape.Rectangle(1, 1, 64, 12, jar.Color(0, 0, 0, 0), 1, jar.Color.White)
--]]

local testImage = jar.Image()
if not testImage:LoadFromFile("textures/tests/me2.jpg") then
	error("Could not load test image!")
end
local testSprite = jar.Sprite(testImage)

local moar = true
local x = 400
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
	g_testWindow:Display()
end

--g_CVarManager:SaveCVars()
print("==== Test Suite End ====")