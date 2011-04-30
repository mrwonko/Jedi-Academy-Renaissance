require("GameState.lua")

IntroState =
{
}

function IntroState:OnEvent(event)
	if event.Type == jar.Event.KeyPressed then
		if event.Key.Code == jar.Key.Escape or
		event.Key.Code == jar.Key.Return or
		event.Key.Code == jar.Key.Space then
			g_currentState = GameState
			return true
		end
	end
	return false --has not been used
end

function IntroState:Update(deltaT)
end

local arial = g_FontManager:GetFont("arial", true)

function IntroState:RenderTo(target)
end