GameState =
{
}

local paused = false
function GameState:OnEvent(event)
	if event.Type == jar.Event.KeyPressed then
		if event.Key.Code == jar.Key.Escape then
			if paused then
				g_currentState = MainMenuState
				paused = false
			else
				paused = true
			end
			return true
		elseif event.Key.Code == jar.Key.Return then
			if paused then
				paused = false
				return true
			end
		end
	end
	return false --has not been used
end

function GameState:Update(deltaT)
	if paused then return end
end

local arial = g_FontManager:GetFont("arial", true)

local pauseText = jar.Text(arial) -- second parameter: throw error on failure
pauseText:SetText("Pause - Escape to Quit, Return to continue")
pauseText:SetPosition( (g_Window:GetWidth() - pauseText:GetWidth()) / 2, (g_Window:GetHeight() - pauseText:GetHeight()) / 2)
local pauseRect = jar.Shape.Rectangle(0, 0, g_Window:GetWidth(), g_Window:GetHeight(), jar.Color(0, 0, 0, 127), 0, jar.Color.Black)

function GameState:RenderTo(target)
	target:Clear(jar.Color.Red)
	if paused then
		target:Draw(pauseRect)
		target:Draw(pauseText)
	end
end