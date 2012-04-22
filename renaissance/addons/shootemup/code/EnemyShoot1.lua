require("Enemy.lua")
require("ShotLaser.lua")

EnemyShoot1 = Enemy:New(
{
	image = g_ImageManager:GetImage("textures/lores/enemy_shoot"),
	movedir = jar.Vector2f(-0.02, .02),
	crashDamage = 10,
	health = 20,
	score = 15,
	size = 10,
	weapon = ShotLaser,
},
true)

function EnemyShoot1:New(info)
	local obj = Enemy.New(self, info)
	--TODO: replace outdated Resize code
	--obj.sprite:Resize(obj.size, obj.size)
	return obj
end

function EnemyShoot1:Move(deltaT)
	self.sprite:Move(self.movedir*deltaT)
	if self.sprite:GetPosition().Y > 100-self.size/2 then
		self.movedir = jar.Vector2f(self.movedir.X, -math.abs(self.movedir.Y))
	elseif self.sprite:GetPosition().Y < self.size/2 then
		self.movedir = jar.Vector2f(self.movedir.X, math.abs(self.movedir.Y))
	end
end