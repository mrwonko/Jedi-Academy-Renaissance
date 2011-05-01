require("Shot.lua")

ShotLaser = Shot:New({
	speed = 0.2,
	image = g_ImageManager:GetImage("textures/lores/shot_laser.png", true),
	damage = 10,
	cooldown = 500,
	shotsounds = {
		g_SoundManager:GetSound("sound/laser01"),
		g_SoundManager:GetSound("sound/laser01_2"),
		g_SoundManager:GetSound("sound/laser01_3"),
	},
	scale = 0.5,
}, true) --true = silent - no shot sound played on creation.