require("Enemy.lua")
require("ShotLaser.lua")

EnemyShoot2 = Enemy:New(
{
	image = g_ImageManager:GetImage("textures/lores/enemy_shoot"),
	movedir = jar.Vector2f(-0.02, 0),
	crashDamage = 10,
	health = 20,
	score = 15,
	size = 10,
	weapon = ShotLaser,
},
true)

function EnemyShoot2:New(info)
	local obj = Enemy.New(self, info)
	--TODO: replace outdated Resize code
	--obj.sprite:Resize(obj.size, obj.size)
	return obj
end

function EnemyShoot2:Move(deltaT)
	self.sprite:Move(self.movedir*deltaT)
end