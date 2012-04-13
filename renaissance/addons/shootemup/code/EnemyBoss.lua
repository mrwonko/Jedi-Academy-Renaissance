require("Enemy.lua")
require("ShotLaser.lua")

EnemyBoss = Enemy:New(
{
	image = g_ImageManager:GetImage("textures/lores/enemy_scrot"),
	movedir = jar.Vector2f(-0.02, 0),
	crashDamage = 10,
	health = 400,
	score = 200,
	size = 20,
	weapon = ShotLaserBlue,
	moveup = true,
	particleInfo =
	{
		images =
		{
			g_ImageManager:GetImage("textures/lores/scrot_scrap1"),
			g_ImageManager:GetImage("textures/lores/scrot_scrap2"),
			g_ImageManager:GetImage("textures/lores/scrot_scrap3"),
			g_ImageManager:GetImage("textures/lores/explosion01"),
			g_ImageManager:GetImage("textures/lores/explosion01_2"),
			g_ImageManager:GetImage("textures/lores/explosion01_3"),
			g_ImageManager:GetImage("textures/lores/explosion01"),
			g_ImageManager:GetImage("textures/lores/explosion01_2"),
			g_ImageManager:GetImage("textures/lores/explosion01_3"),
		},
		minNum = 9,
		maxNum = 30,
		minSpeed = 0.01,
		maxSpeed = 0.04,
		lifetime = 4000,
		size = 5,
	}
},
true)

function EnemyBoss:New(info)
	local obj = Enemy.New(self, info)
	--TODO: replace outdated Resize code
	--obj.sprite:Resize(obj.size, obj.size)
	return obj
end

function EnemyBoss:Move(deltaT)
	if self.sprite:GetPosition().X > 150 then
		self.sprite:Move(self.movedir*deltaT)
	else
		if self.moveup and self.sprite:GetPosition().Y < 10 then
			self.moveup = false
		elseif not self.moveup and self.sprite:GetPosition().Y > 90 then
			self.moveup = true
		end
		if self.moveup then
			self.sprite:Move(0, -0.02 * deltaT)
		else
			self.sprite:Move(0, 0.02 * deltaT)
		end
	end
end