require("Particle.lua")

Enemy = {
	health = 10,
	crashDamage = 10, --damage when they fly into the player
	image = nil,
	weapon = false,
	cooldown = 1000, --depends on weapon used
	size = 10,
	score = 1,
	deathsounds =
	{
		g_SoundManager:GetSound("sound/explosion01"),
		g_SoundManager:GetSound("sound/explosion01_2"),
		g_SoundManager:GetSound("sound/explosion01_3"),
	},
	particleInfo =
	{
		images =
		{
			g_ImageManager:GetImage("textures/lores/explosion01"),
			g_ImageManager:GetImage("textures/lores/explosion01_2"),
			g_ImageManager:GetImage("textures/lores/explosion01_3"),
		},
		minNum = 10,
		maxNum = 30,
		minSpeed = 0.01,
		maxSpeed = 0.02,
		lifetime = 2000,
		size = 3,
	}
}

function Enemy:New(info, template)
	local obj = info or {}
	setmetatable(obj, self)
	self.__index = self
	
	if obj.image then
		obj.sprite = jar.Sprite(obj.image)
		obj.sprite:SetOrigin(obj.image:GetWidth()/2, obj.image:GetHeight()/2)
		obj.sprite:Resize(obj.size, obj.size)
	else
		error("no image set!", 2)
	end
	if obj.position then
		obj.sprite:SetPosition(obj.position)
		obj.position = nil
	end
	
	if not template then
		table.insert(g_Gamefield.enemies, obj)
	end
	
	return obj
end

function Enemy:Damage(amount)
	self.health = self.health - amount
	if self.health <= 0 then
		self:Die()
	end
end

function Enemy:Die()
	local sound = GetSound()
	sound:SetBuffer(self.deathsounds[math.random(#self.deathsounds)])
	sound:SetPosition(0, 0, 0)
	sound:Play()
	
	self.particleInfo.position = self.sprite:GetPosition()
	CreateParticles(self.particleInfo)
end

function Enemy:Update(deltaT)
	self:Move(deltaT)
	if self.weapon then
		self.cooldown = self.cooldown - deltaT
		do return end
		if self.cooldown <= 0 then
			local shot = self.weapon:New{
				position = self.sprite:GetPosition(),
				dir = -1,
			}
			table.insert(g_Gamefield.shots, shot)
			self.cooldown = self.weapon.cooldown
		end
	end
end

function Enemy:Move(deltaT)
end

function Enemy:Render(target)
	target = target or g_Window
	target:Draw(self.sprite)
end