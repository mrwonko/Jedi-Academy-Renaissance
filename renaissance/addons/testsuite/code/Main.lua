require("InstructionInterpreter.lua")
require("CCommandManager.lua")
require("WordWrap.lua")
require("TextureManager.lua")

local textureManager = TextureManager()

local glPushMatrix = jar.gl.PushMatrix
local glPopMatrix = jar.gl.PopMatrix
local glPerspective = jar.gl.Perspective
local glLoadIdentity = jar.gl.LoadIdentity
local glMatrixMode = jar.gl.MatrixMode
local GL_MODELVIEW = jar.gl.enum.MODELVIEW
local GL_PROJECTION = jar.gl.enum.PROJECTION
local glColor = jar.gl.Color
local glRotate = jar.gl.Rotate
local glTranslate = jar.gl.Translate

local Prepare3DRender = jar.Prepare3DRender
local Prepare2DRender = jar.Prepare2DRender

local model = jar.g2.Model()
local success, err = model:LoadFromFile("models/test/blaster_pistol_w.glm") -- sorry, can't include this due to copyrights - will make a custom test model in time.
if not success then error(err) end
if not model:UploadToGPU() then error("Can't upload to GPU") end

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
	--print("Core Update: " .. jar.GetTime() - startTime .. "ms")
	
	g_TestWindow:Clear(jar.Color.Black)
	g_TestWindow:Clear(jar.Color.Magenta)
	
	--g_TestWindow:Draw(testSprite)
	--g_TestWindow:Draw(testText)
	
	--   prepare for 3D drawing
	Prepare3DRender(90, g_TestWindow:GetSize().X/g_TestWindow:GetSize().Y, 1, 1024)
	--[[
	glRotate(-90, 1, 0, 0) --rotating 90° ccw around X should turn Z from Back into Up
	glRotate(-90, 0, 0, 1) --rotating 90° ccw around Z should turn X from Right into Back
	glTranslate(-64, 0, 0)
	--]]
	
	--   3d drawing!
	glTranslate(-10, 0, -20)
	--jar.RenderTriangleOfDeath()
	
	---[[
	glColor(jar.Color.Red)
	model:Render()
	
	glTranslate(20, 0, 0)
	glColor(jar.Color.Blue)
	model:Render()
	--]]
	--jar.RenderTriangleOfDeath()
	
	--   reset to 2D drawing
	Prepare2DRender(g_TestWindow)
	
	--console last since it's an overlay
	g_Console:RenderTo(g_TestWindow)
	
	g_TestWindow:Display()
	if not hasFocus then
		-- sleep 5 ms
		jar.Sleep(5)
	end
end

-- ensure model gets deleted before window (and OpenGL context with it)
model = nil
-- or I could just model:DeleteFromGPU()...
collectgarbage()

g_CVarManager:SaveCVars()
g_BindManager:Save()
print("==== Test Suite End ====")
