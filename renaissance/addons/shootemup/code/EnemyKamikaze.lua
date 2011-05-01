require("Enemy.lua")

EnemyKamikaze = Enemy:New(
{
	image = g_ImageManager:GetImage("textures/lores/enemy_blink"),
	timePerFrame = 250,
	nextFrameIn = 250,
	curFrame = 0,
	numFrames = 2,
	speed = 0.05,
	speedY = 0.02,
	crashDamage = 20,
	health = 30,
	score = 10,
},
true)

function EnemyKamikaze:New(info)
	local obj = Enemy.New(self, info)
	obj.sprite:SetSubRect(jar.IntRect(0, 0, 16, 16))
	obj.sprite:Resize(obj.size, obj.size)
	obj.sprite:SetOrigin(obj.image:GetWidth()/4, obj.image:GetHeight()/2) --anim hack, image is too wide
	return obj
end

function EnemyKamikaze:Update(deltaT)
	Enemy.Update(self, deltaT)
	self.nextFrameIn = self.nextFrameIn - deltaT
	while self.nextFrameIn < 0 do
		self.nextFrameIn = self.nextFrameIn + self.timePerFrame
		self.curFrame = self.curFrame + 1
		if self.curFrame >= self.numFrames then
			self.curFrame = 0
		end
		self.sprite:SetSubRect(jar.IntRect(self.curFrame*16, 0, 16, 16))
	end
end

function EnemyKamikaze:Move(deltaT)
	if self.sprite:GetPosition().y > g_Gamefield.player.sprite:GetPosition().y + 2 then
		self.sprite:Move(-self.speed*deltaT, -self.speedY*deltaT)
	elseif self.sprite:GetPosition().y < g_Gamefield.player.sprite:GetPosition().y - 2 then
		self.sprite:Move(-self.speed*deltaT, self.speedY*deltaT)
	else
		self.sprite:Move(-self.speed*deltaT, 0)
	end
end