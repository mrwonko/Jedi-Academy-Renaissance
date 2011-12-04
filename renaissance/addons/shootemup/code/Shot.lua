require("Particle.lua")

Shot =
{
	speed = 0.5,
	image = nil,
	damage = 1,
	dir = 1, --1 = right, -1 = left
	dead = false,
	scale = 1,
	cooldown = 1000, -- AI only
	hitsounds =
	{
		g_SoundManager:GetSound("sound/hit01"),
		g_SoundManager:GetSound("sound/hit01_2"),
		g_SoundManager:GetSound("sound/hit01_3"),
	},
	particleInfo =
	{
		images =
		{
			g_ImageManager:GetImage("textures/lores/shot_laser_impact"),
		},
		minNum = 3,
		maxNum = 6,
		minSpeed = 0.02,
		maxSpeed = 0.01,
		lifetime = 300,
		size = 1,
	}
}

function Shot:New(info, silent)
	local obj = info or {}
	setmetatable(obj, self)
	self.__index = self
	
	if obj.image then
		obj.image:SetSmooth(false)
		obj.sprite = jar.Sprite(obj.image)
		if obj.dir < 0 then
			obj.sprite:FlipX(true)
		end
		obj.sprite:SetOrigin(obj.image:GetWidth()/2, obj.image:GetHeight()/2)
		obj.sprite:Scale(obj.scale, obj.scale)
	else
		error("no image set!", 2)
	end
	if obj.position then
		obj.sprite:SetPosition(obj.position)
		obj.position = nil
	end
	
	if type(obj.shotsounds) == "table" and not silent then
		local sound = GetSound()
		sound:SetBuffer(obj.shotsounds[math.random(#obj.shotsounds)])
		sound:SetPosition(-obj.dir, 0, 0)
		sound:SetVolume(g_CVarManager:GetCVarValue("snd_effectsvolume") or 100)
		sound:Play()
	end
	
	return obj
end

function Shot:Update(deltaT)
	self.sprite:Move(self.speed*deltaT*self.dir, 0)
	local pos = self.sprite:GetPosition().x
	if pos > self.image:GetWidth()/2+200 or pos < -self.image:GetWidth()/2 then
		self.dead = true
	end
	if self.shotsound and self.shotsound:GetStatus() == jar.Sound.Stopped then
		self.shotsound = nil
	end
end

function Shot:Render()
	if not self.dead then
		g_Window:Draw(self.sprite)
	end
end

function Shot:Hit()
	self.dead = true
	local sound = GetSound()
	sound:SetBuffer(self.hitsounds[math.random(#self.hitsounds)])
	sound:SetPosition(0, 0, 0)
	sound:Play()
	
	self.particleInfo.position = self.sprite:GetPosition()
	CreateParticles(self.particleInfo)
end