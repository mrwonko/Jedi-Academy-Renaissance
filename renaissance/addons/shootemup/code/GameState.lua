require("Gamefield.lua")

GameState =
{
	GameOver = false,
	GameWon = false,
}


local arial = g_FontManager:GetFont("arial", true)

local pauseText = jar.Text(arial) -- second parameter: throw error on failure
pauseText:SetText("Pause - Escape to Quit, Return to continue")
pauseText:SetPosition( (g_Window:GetWidth() - pauseText:GetWidth()) / 2, (g_Window:GetHeight() - pauseText:GetHeight()) / 2)

local gameoverText = jar.Text(arial, 20) -- second parameter: throw error on failure
gameoverText:SetText("Game Over")
gameoverText:SetPosition( (g_Window:GetWidth() - gameoverText:GetWidth()) / 2, (g_Window:GetHeight() - gameoverText:GetHeight()) / 2)

local wonText = jar.Text(arial, 20) -- second parameter: throw error on failure
wonText:SetText("You are a winner!")
wonText:SetPosition( (g_Window:GetWidth() - wonText:GetWidth()) / 2, (g_Window:GetHeight() - wonText:GetHeight()) / 2)

local pauseRect = jar.Shape.Rectangle(0, 0, g_Window:GetWidth(), g_Window:GetHeight(), jar.Color(0, 0, 0, 127), 0, jar.Color.Black)

-- calculate the view size
local guiTopHeight = pauseText:GetHeight()
local availableGamefieldSpace = jar.Vector2f(g_Window:GetWidth(), g_Window:GetHeight() - guiTopHeight)
local availableAspectRatio = availableGamefieldSpace.x / availableGamefieldSpace.y
local gamefieldSpace = jar.Vector2f()
local viewport = jar.FloatRect()
viewport.top = guiTopHeight / g_Window:GetHeight()
if availableAspectRatio > 2 then -- wider than necessary - black bars left & right
	gamefieldSpace.x = 2*availableGamefieldSpace.y
	gamefieldSpace.y = availableGamefieldSpace.y
	viewport.left = (availableGamefieldSpace.x - gamefieldSpace.x) / 2  / g_Window:GetWidth()
else --not wide enough - black bar at bottom
	gamefieldSpace.x = availableGamefieldSpace.x
	gamefieldSpace.y = availableGamefieldSpace.x/2
	viewport.left = 0
end
viewport.width = gamefieldSpace.x / g_Window:GetWidth()
viewport.height = gamefieldSpace.y / g_Window:GetHeight()
availableAspectRatio = nil
availableGamefieldSpace = nil

local gamefieldView = jar.View(jar.FloatRect(0, 0, 200, 100))
gamefieldView:SetViewport(viewport)
viewport = nil

g_Gamefield = 0

local paused = false

function GameState:LoseFocus()
	if not self.GameOver and not self.GameWon then
		paused = true
	end
end

function GameState:OnEvent(event)
	if event.Type == jar.Event.KeyPressed then
		if event.Key.Code == jar.Key.Escape then
			if paused then
				SetCurrentState(MainMenuState)
				paused = false
			elseif self.GameOver or self.GameWon then
				self.GameOver = false
				self.GameWon = false
				SetCurrentState(MainMenuState)
				paused = false --just in case
			else
				paused = true
			end
			return true
		elseif event.Key.Code == jar.Key.Return and paused then
			paused = false
			return true
		elseif event.Key.Code == jar.Key.Return and (self.GameOver or self.GameWon) then
			self.GameOver = false
			self.GameWon = false
			SetCurrentState(MainMenuState)
			paused = false --just in case
		elseif not paused and not self.GameOver and not self.GameWon then
			return g_Gamefield:OnKeyDown(event.Key.Code)
		end
	end
	return false --has not been used
end

function GameState:RenderTo(target)
	-- render top gui
	-- render gamefield
	g_Gamefield:RenderGUI()
	g_Window:SetView(gamefieldView)
	g_Gamefield:RenderGamefield()
	g_Window:SetView(g_Window:GetDefaultView())
	-- pause overlay if applicable
	if paused or self.GameOver or self.GameWon then
		target:Draw(pauseRect)
		if paused then
			target:Draw(pauseText)
		elseif self.GameOver then
			target:Draw(gameoverText)
		else
			target:Draw(wonText)
		end
	end
end

function GameState:Update(deltaT)
	if g_Console.isOpened then paused = true end
	if paused or self.GameOver or self.GameWon then return end
	g_Gamefield:Update(deltaT)
end

function GameState:OnStart()
	--init
	g_Gamefield = Gamefield:New()
	self.currentLevel = 1
	--todo: level stuff. enemies first.
	--music
	--friggin wolfram tones terms of use
	--[[local buf = g_SoundManager:GetSound("sound/mus_game.ogg")
	if buf then
		g_Music:SetBuffer(buf)
		g_Music:SetVolume(g_CVarManager:GetCVarValue("snd_musicvolume") or 100)
		g_Music:Play()
	end
	--]]
end

function GameState:OnEnd()
	--deinit
	g_Gamefield = nil
end