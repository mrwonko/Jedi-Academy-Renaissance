Star = {}
local starimages =
{
	g_ImageManager:GetImage("textures/lores/star01.png", true),
	g_ImageManager:GetImage("textures/lores/star02.png", true),
	g_ImageManager:GetImage("textures/lores/star03.png", true),
	g_ImageManager:GetImage("textures/lores/star03.png", true),
}
function Star:New()
	local obj =
	{
		sprite = jar.Sprite(starimages[math.random(1, 3)], jar.Vector2f(200, math.random()*100)),
		speed = -math.random(1, 20)/500,
	}
	setmetatable(obj, self)
	self.__index = self
	return obj
end

function Star:IsDead()
	return self.sprite:GetPosition().X < -4
end

function Star:Update(deltaX)
	self.sprite:Move(self.speed, 0)
end

function Star:Render()
	g_Window:Draw(self.sprite)
end