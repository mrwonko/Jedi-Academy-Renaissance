require("Enemy.lua")

EnemyAsteroid = Enemy:New(
{
	image = g_ImageManager:GetImage("textures/lores/enemy_asteroid"),
	movedir = jar.Vector2f(-0.02, 0),
	crashDamage = 10,
	health = 20,
	score = 5,
	size = 10,
	particleInfo =
	{
		images =
		{
			g_ImageManager:GetImage("textures/lores/asteroid_splinter"),
			g_ImageManager:GetImage("textures/lores/asteroid_splinter_2"),
		},
		minNum = 4,
		maxNum = 8,
		minSpeed = 0.01,
		maxSpeed = 0.02,
		lifetime = 2000,
		size = 3,
	}
},
true)

function EnemyAsteroid:New(info)
	local obj = Enemy.New(self, info)
	--TODO: replace outdated Resize code
	--obj.sprite:Resize(obj.size, obj.size)
	return obj
end

function EnemyAsteroid:Move(deltaT)
	self.sprite:Move(self.movedir*deltaT)
end