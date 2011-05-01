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
}

function Shot:New(info, silent)
	local obj = info or {}
	setmetatable(obj, self)
	self.__index = self
	
	if obj.image then
		obj.image:SetSmooth(false)
		obj.sprite = jar.Sprite(obj.image)
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
		obj.shotsound = jar.Sound()
		obj.shotsound:SetBuffer(obj.shotsounds[math.random(#obj.shotsounds)])
		obj.shotsound:SetPosition(-obj.dir, 0, 0)
		obj.shotsound:SetVolume(g_CVarManager:GetCVar("snd_effectsvolume") or 100)
		obj.shotsound:Play()
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
	local sound = jar.Sound()
	sound:SetBuffer(self.hitsounds[math.random(#self.hitsounds)])
	sound:Play()
	table.insert(g_Gamefield.managedSounds, sound)
	--todo: impact effect!
end