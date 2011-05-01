Enemy = {
	health = 10,
	image = nil,
	speed = 0.1,
}

function Enemy:New(info)
	local obj = info or {}
	setmetatable(obj, self)
	self.__index = self
	
	if obj.image then
		obj.sprite = jar.Sprite(obj.image)
		obj.sprite:SetOrigin(obj.image:GetWidth()/2, obj.image:GetHeight()/2)
	else
		error("no image set!", 2)
	end
	if obj.position then
		obj.sprite:SetPosition(obj.position)
		obj.position = nil
	end
	
	table.insert(g_Gamefield.enemies, obj)
	
	return obj
end

function Enemy:Damage(amount)
	self.health = self.health - amount
end

function Enemy:Update(deltaT)
	self:Move(deltaT)
end

function Enemy:Move(deltaT)
end