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
}, true) --true = silent - no shot sound played on creation.