require("GameState.lua")

IntroState =
{
}

function IntroState:OnEvent(event)
	if event.Type == jar.Event.KeyPressed then
		if event.Key.Code == jar.Key.Escape or
		event.Key.Code == jar.Key.Return or
		event.Key.Code == jar.Key.Space then
			SetCurrentState(GameState)
			return true
		end
	end
	return false --has not been used
end

local img = g_ImageManager:GetImage("textures/intro")
local spr = jar.Sprite(img)
local moveIn = 3000
local startIn = 0

function IntroState:OnStart()
	spr:Resize(g_Window:GetWidth()*2, g_Window:GetWidth())
	spr:SetPosition(0, -(g_Window:GetWidth()-g_Window:GetHeight()) / 2)
	moveIn = 3000
	startIn = 0
end

function IntroState:Update(deltaT)
	if moveIn > 0 then
		moveIn = moveIn - deltaT
	elseif startIn == 0 then
		spr:Move(-0.2*deltaT, 0)
		if spr:GetPosition().x < -g_Window:GetWidth() then
			spr:SetX(-g_Window:GetWidth())
			startIn = 1000
		end
	elseif startIn > 0 then
		startIn = startIn - deltaT
		if startIn == 0 then
			startIn = -1
		end
	else
		SetCurrentState(GameState)
	end
end

function IntroState:RenderTo(target)
	target:Draw(spr)
end