-- particle, lives totalLifetime miliseconds, gets linearly more transparent

Particle =
{
	direction = nil,
	totalLifetime = 1000,
	image = nil,
	size = 4,
	angle = 0,
}

ParticleManager = 
{
	particles = {},
}

function Particle:New(info)
	local obj = info or {}
	setmetatable(obj, self)
	self.__index = self
	
	if not obj.image then
		error("no img omfg!", 2)
	end
	
	--init
	obj.sprite = jar.Sprite(obj.image)
	obj.sprite:SetPosition(obj.position)
	obj.sprite:SetOrigin(obj.sprite:GetSize()/2)
	obj.sprite:Resize(obj.size, obj.size)
	obj.sprite:Rotate(obj.angle*180/math.pi)
	obj.position = nil --I use sprite's position
	obj.lifetime = obj.totalLifetime
	
	table.insert(ParticleManager.particles, obj)
	
	return obj
end

function Particle:Update(deltaT)
	self.sprite:Move( (self.direction or jar.Vector2f()) * deltaT )
	self.lifetime = self.lifetime - deltaT
	self.sprite:SetColor(jar.Color(255, 255, 255, 255*self.lifetime/self.totalLifetime))
end

function ParticleManager:Update(deltaT)
	local i = 1
	while true do
		if i > #self.particles then
			break
		end
		local particle = self.particles[i]
		particle:Update(deltaT)
		if particle.lifetime <= 0 then
			table.remove(self.particles, i)
		else
			i = i + 1
		end
	end
end

function ParticleManager:Render()
	for _, particle in ipairs(self.particles) do
		g_Window:Draw(particle.sprite)
	end
end

function CreateParticles(info)
	--todo: check for validity etc.
	for i = 1, math.random(info.minNum, info.maxNum) do
		local angle = math.random()*2*math.pi --somewhat redundant but I need this for sprite rotation
		local speed = math.random()*(info.maxSpeed-info.minSpeed)+info.minSpeed
		Particle:New{
			position = info.position,
			totalLifetime = info.lifetime,
			image = info.images[math.random(#info.images)],
			size = info.size,
			angle = angle,
			direction = jar.Vector2f(math.cos(angle)*speed, math.sin(angle)*speed),
		}
	end
end