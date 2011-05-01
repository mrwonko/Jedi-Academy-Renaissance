require("Shot.lua")

ShotLaserBlue = Shot:New({
	speed = 0.1,
	image = g_ImageManager:GetImage("textures/lores/shot_laser_blue.png", true),
	damage = 20,
	shotsounds = {
		g_SoundManager:GetSound("sound/laser02"),
		g_SoundManager:GetSound("sound/laser02_2"),
		g_SoundManager:GetSound("sound/laser02_3"),
	},
	scale = 0.8,
	particleInfo =
	{
		images =
		{
			g_ImageManager:GetImage("textures/lores/shot_laser_blue_impact"),
		},
		minNum = 3,
		maxNum = 6,
		minSpeed = 0.02,
		maxSpeed = 0.01,
		lifetime = 300,
		size = 1,
	}
}, true) --true = silent - no shot sound played on creation.