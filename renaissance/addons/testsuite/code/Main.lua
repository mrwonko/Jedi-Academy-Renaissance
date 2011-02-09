require("InstructionInterpreter.lua")
require("CCommandManager.lua")
require("WordWrap.lua")

print("==== Test Suite Start ====")
print("")

local testImage = jar.Image()
if not testImage:LoadFromFile("textures/tests/me2.jpg") then
	error("Could not load test image!")
end
local testSprite = jar.Sprite(testImage)

local testFont = g_FontManager:GetFont("times", true) -- seconds parameter: throw error on failure
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
textBox = jar.Shape.Rectangle(10, 10, g_TestWindow:GetWidth()-10, 10+testFont:GetHeight(), jar.Color(0, 0, 0, 0), 1, jar.Color.Yellow)
textBox = jar.Shape.Rectangle(10, 10, g_TestWindow:GetWidth()-10, g_TestWindow:GetHeight()-10, jar.Color(0, 0, 0, 0), 1, jar.Color.Yellow)

--testText:SetFontSize(8)

local moar = true
local x = 400
local aLittleCircle = jar.Shape.Circle(0, 0, 32, jar.Color(0, 0, 0, 0), 1, jar.Color.Red)
aLittleCircle:SetY(300)
aLittleCircle:SetX(x)

local running = true
g_CCommandManager:RegisterCommand
{
	name = "quit",
	description = "Exits the program",
	Execute = function(self)
		running = false
	end,
}

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
	
	g_InstructionInterpreter:Update(frametime)
	
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

g_CVarManager:SaveCVars()
g_BindManager:Save()
print("==== Test Suite End ====")