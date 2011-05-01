require("KeyBindings.lua")
require("KeyCVars.lua")
require("KeyManager.lua")
require("ShotLaser.lua")
require("ShotLaserBlue.lua")

Player =
{
	movespeed = 0.05, --units per millisecond
	currentWeapon = 1,
	health = 100,
	maxHealth = 100,
}

local playerimg = g_ImageManager:GetImage("textures/lores/player.png", true) -- 2nd parameter: throw error on failure?
playerimg:SetSmooth(false)

function Player:New()
	local obj =
	{
		sprite = jar.Sprite(playerimg),
		availableWeapons =
		{
			ShotLaser,
			ShotLaserBlue, --todo: delete this.
		}
	}
	obj.sprite:Resize(10, 10)
	obj.sprite:SetOrigin(0, playerimg:GetHeight()/2)
	obj.sprite:SetPosition(0, 50)
	
	setmetatable(obj, self)
	self.__index = self
	
	return obj
end

function Player:GetPosition() return self.sprite:GetPosition() end
function Player:SetPosition(x, y) return self.sprite:SetPosition(x, y) end
function Player:SetX(x) return self.sprite:SetX(x) end
function Player:SetY(y) return self.sprite:SetY(y) end
function Player:Move(x, y) return self.sprite:Move(x, y) end

function Player:Render()
	g_Window:Draw(self.sprite)
end

function Player:AddWeapon(weapon)
	for _, avweap in ipairs(self.availableWeapons) do
		if avweap == weapon then
			return
		end
	end
	table.insert(self.availableWeapons, weapon)
end

function Player:AddAndSelectWeapon(weapon)
	for i, avweap in ipairs(self.availableWeapons) do
		if avweap == weapon then
			self.currentWeapon = i
			return
		end
	end
	table.insert(self.availableWeapons, weapon)
	self.currentWeapon = #self.availableWeapons
end

function Player:Update(deltaT)
	if KeyManager:IsKeyDown(GetKey("up") or jar.Key.Up) then
		self:Move(0, -self.movespeed*deltaT)
		if self:GetPosition().y < 5 then self:SetY(5) end
	end
	if KeyManager:IsKeyDown(GetKey("down") or jar.Key.Down) then
		self:Move(0, self.movespeed*deltaT)
		if self:GetPosition().y > 95 then self:SetY(95) end
	end
end

function Player:OnKeyDown(key)
	if key == (GetKey("shoot") or jar.Key.Space) then
		local pos = jar.Vector2f(self.sprite:GetPosition().x, self.sprite:GetPosition().y)
		pos.x = pos.x + 10
		local shot = self.availableWeapons[self.currentWeapon]:New{position = pos, dir=1,}
		table.insert(g_Gamefield.shots, shot)
		return true
	end
	if key == (GetKey("nextweapon") or jar.Key.Right) then
		self.currentWeapon = self.currentWeapon + 1
		if self.currentWeapon > #self.availableWeapons then
			self.currentWeapon = 1
		end
		return true
	end
	if key == (GetKey("lastweapon") or jar.Key.Left) then
		self.currentWeapon = self.currentWeapon - 1
		if self.currentWeapon < 1 then
			self.currentWeapon = #self.availableWeapons
		end
		return true
	end
	return false
end

function Player:Damage(amount)
	self.health = self.health - amount
end