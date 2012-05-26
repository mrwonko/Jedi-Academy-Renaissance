require("InstructionInterpreter.lua")
require("CCommandManager.lua")
require("WordWrap.lua")

print("==== Test Suite Start ====")
print("")

if not testsuiteInitialized then
	error("Not correctly initialized!")
end
testsuiteInitialized = nil

local testImage = g_ImageManager:GetImage("textures/tests/me2.jpg", true) -- true = throw error on failure, not usually wanted
local testSprite = jar.Sprite(testImage)

local testFont = g_FontManager:GetFont("times", true) -- seconds parameter: throw error on failure
--testFont:SetTabWidth(4)

local wrappedText = table.concat(WordWrap([[^1I just added font/text rendering to my engine. (The rectangle is a word wrap test visualization. This text should not be right of it.)

^2I could've just used SFML's font system, ^3but I want to use the same file formats used in ^4Jedi Knight: Jedi Academy ^3(including fonts). ^5It's basically just an image and a file containing information about the positions of the characters (as well as UV mapping).

^6Now I've got everything I need to add an ingame console: low-level input handling, console commands, console variables, font rendering, shape rendering...

^7The console will be a great tool for debugging and developing.

Special characters also work, as far as I can tell: Für Khazad-Dûm! Öhöhö, Spaß! (Ja, äußerst sinnlos, dieser Text.) ThisTextIsSoLongThatItUsesAWholeLineWhichIsQuiteInsaneButAppropriate(OrSoIThink)TheseBracesRemindMeThatIDon'tPreferablyWrapAtSuchSpecialCharactersLike(or)or'or,etc.

now		I		also
added	support	for
tabstops.]], testFont, nil, g_TestWindow:GetSize().X-20), "\n")

-- test: deriving from a C++ class (Drawable2D)

class "TextTest" (jar.Drawable2D)

function TextTest:__init()
	-- important: call base class constructor!
	jar.Drawable2D.__init(self)
	
	local testText = jar.Text(testFont)
	testText:SetText(wrappedText)
	testText:SetPosition(10, 10)
	local textBox = jar.RectangleShape(jar.Vector2f(g_TestWindow:GetSize().X-20, g_TestWindow:GetSize().Y-20))
	textBox:SetPosition(10, 10)
	textBox:SetFillColor(jar.Color.Transparent)
	textBox:SetOutlineThickness(1)
	textBox:SetOutlineColor(jar.Color.Yellow)
	
	self.testText = testText
	self.textBox = textBox
end

function TextTest:Draw(target, states)
	target:Draw(self.textBox)
	target:Draw(self.testText)
end

local testText = TextTest()

--testText:SetFontSize(8)

local moar = true
local x = 400
local aLittleCircle = jar.CircleShape(32)
aLittleCircle:SetFillColor(jar.Color.Transparent)
aLittleCircle:SetOutlineColor(jar.Color.Red)
aLittleCircle:SetOutlineThickness(1)
aLittleCircle:SetY(300)
aLittleCircle:SetOrigin(32, 32)
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

local hasFocus = true
local function HandleEvents()
	g_EventListenerStack:PreEvent()
	while true do
		local success, event = jar.EventManager.GetSingleton():GetEvent()
		if not success then
			-- no more events available
			break
		end
		
		if event.Type == jar.Event.Closed then
			running = false
		elseif event.Type == jar.Event.GainedFocus then
			hasFocus = true
			g_TestWindow:SetMouseCursorVisible(false)
		elseif event.Type == jar.Event.LostFocus then
			hasFocus = false
			g_TestWindow:SetMouseCursorVisible(true)
		elseif hasFocus then
			g_EventListenerStack:OnEvent(event)
		end
	end
	g_EventListenerStack:PostEvent()
end

-- Sound test
local Sound = jar.Sound()
local SoundBuffer = jar.SoundBuffer()
if not SoundBuffer:LoadFromFile("sound/beep.wav") then
	error("Could not laod \"sound/beep.wav\"")
end
Sound:SetBuffer(SoundBuffer)

local lastFrametime = jar.GetTime()

local middle = jar.Vector2i(g_TestWindow:GetSize().X \ 2, g_TestWindow:GetSize().Y \ 2) -- \ is div
while running do
	HandleEvents()
	if hasFocus then
		g_TestWindow:SetCursorPosition(middle)
	end
	
	local frametime = jar.GetTime()
	assert(frametime >= lastFrametime)
	if frametime == lastFrametime then -- I clamp the frame rate to a thousand fps because my time is in milliseconds.
		jar.Sleep(1)
		frametime = jar.GetTime()
	end
	local deltaT = math.min(frametime - lastFrametime, 100) -- no longer than 100ms per frame (e.g. breakpoints in debugging might screw up timings otherwise)
	lastFrametime = frametime
	
	g_InstructionInterpreter:Update(deltaT)
	
	local startTime = jar.GetTime()
	jar.Core.GetSingleton():Update(deltaT)
	print("Core Update: " .. jar.GetTime() - startTime .. "ms")
	
	aLittleCircle:SetX(x)
	
	
	if moar then
		x = x + 0.1 * deltaT
	else
		x = x - 0.1 * deltaT
	end
	
	if x <= 50 or x >= 750 then
		if moar then
			Sound:SetPosition(1, 0, 0)
		else
			Sound:SetPosition(-1, 0, 0)
		end
		--Sound:Play()
		moar = not moar
	end
	
	g_TestWindow:Clear(jar.Color.Black)
	
	--g_TestWindow:Draw(testSprite)
	g_TestWindow:Draw(aLittleCircle)
	g_TestWindow:Draw(testText)
	
	--console last since it's an overlay
	g_Console:RenderTo(g_TestWindow)
	
	g_TestWindow:Display()
	if not hasFocus then
		-- sleep 5 ms
		jar.Sleep(5)
	end
end

g_CVarManager:SaveCVars()
g_BindManager:Save()
print("==== Test Suite End ====")
