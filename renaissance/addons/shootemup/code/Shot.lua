Shot =
{
	speed = 0.5,
	image = nil,
	damage = 1,
	dir = 1, --1 = right, -1 = left
	dead = false,
	scale = 1,
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

function Shot:CheckCollision(sprite)
	local ownsize = self.sprite:GetSize()
	local ownorigin = self.sprite:GetOrigin()
	local ownposition = self.sprite:GetPosition()
	local othersize = sprite:GetSize()
	local otherorigin = sprite:GetOrigin()
	local otherposition = sprite:GetPosition()
	
	local ownRect = jar.FloatRect(ownposition.x-ownorigin.x, ownposition.y-ownorigin.y, ownsize.x, ownsize.y)
	local otherRect = jar.FloatRect(otherposition.x-otherorigin.x, otherposition.y-otherorigin.y, othersize.x, othersize.y)
	
	return ownRect:Intersects(otherRect)
end

function Shot:Render()
	if not self.dead then
		g_Window:Draw(self.sprite)
	end
end

function Shot:Hit()
	self.dead = true
	--todo: explosion!
end