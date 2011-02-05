require("InstructionInterpreter.lua")
require("CCommandManager.lua")
require("WordWrap.lua")

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

print("==== end of InstructionInterpreter tests ====")
print ""

local testImage = jar.Image()
if not testImage:LoadFromFile("textures/tests/me2.jpg") then
	error("Could not load test image!")
end
local testSprite = jar.Sprite(testImage)

local testFont = jar.Font()
if --[[not testFont:LoadFromFile("fonts/anewhope") and]] not testFont:LoadFromFile("fonts/times") then -- I don't distribute anewhope since it's a jka rip.
	error("Could not load font!")
end
--testFont:SetTabWidth(4)

local wrappedText = table.concat(WordWrap([[^1I just added font/text rendering to my engine. (The rectangle is a word wrap test visualization. This text should not be right of it.)

^2I could've just used SFML's font system, ^3but I want to use the same file formats used in ^4Jedi Knight: Jedi Academy ^3(including fonts). ^5It's basically just an image and a file containing information about the positions of the characters (as well as UV mapping).

^6Now I've got everything I need to add an ingame console: low-level input handling, console commands, console variables, font rendering, shape rendering...

^7The console will be a great tool for debugging and developing.

Special characters also work, as far as I can tell: Für Khazad-Dûm! Öhöhö, Spaß! (Ja, äußerst sinnlos dieser Text.) ThisTextIsSoLongThatItUsesAWholeLineWhichIsQuiteInsaneButAppropriate(OrSoIThink)TheseBracesRemindMeThatIDon'tPreferablyWrapAtSuchSpecialCharactersLike(or)or'or,etc.

now		I		also
added	support	for
tabstops.]], testFont, nil, g_TestWindow:GetWidth()-20), "\n")


local testText = jar.Text(testFont)
testText:SetText(wrappedText)
testText:SetPosition(10, 10)
print("Original size: " .. testFont:GetDefaultSize() .. "pt")
textBox = jar.Shape.Rectangle(10, 10, g_TestWindow:GetWidth()-10, 10+testFont:GetHeight(), jar.Color(0, 0, 0, 0), 1, jar.Color.Yellow)
textBox = jar.Shape.Rectangle(10, 10, g_TestWindow:GetWidth()-10, g_TestWindow:GetHeight()-10, jar.Color(0, 0, 0, 0), 1, jar.Color.Yellow)

--testText:SetFontSize(8)

local rect = g_TestWindow:GetView():GetRect()
print("View rect from (" .. rect.left .. ", " .. rect.top .. ") to (" .. rect.right..", "..rect.bottom..")")

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
		--elseif event.Type == jar.Event.KeyPressed and event.Key.Code == jar.Key.Escape then
		--	running = false
		else
			g_EventListenerStack:OnEvent(event)
		end
	end
	
	local frametime = g_TestWindow:GetFrameTime()
	if frametime == 0 then -- I clamp the frame rate to a thousand fps because my time is in milliseconds.
		jar.Sleep(1)
		frametime = g_TestWindow:GetFrameTime()
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
	
	g_TestWindow:Clear(jar.Color.Black)
	
	--g_TestWindow:Draw(aLittleCircle)
	--g_TestWindow:Draw(testRect)
	--g_TestWindow:Draw(testText)
	--g_TestWindow:Draw(testSprite)
	g_TestWindow:Draw(textBox)
	g_TestWindow:Draw(testText)
	
	--console last since it's an overlay
	g_Console:RenderTo(g_TestWindow)
	
	g_TestWindow:Display()
end

--g_CVarManager:SaveCVars()
print("==== Test Suite End ====")