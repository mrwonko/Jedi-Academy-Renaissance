require("Player.lua")
require("Star.lua")

Gamefield = 
{
	nextStarIn = 0,
}

function Gamefield:New()
	local obj =
	{
		player = Player:New(),
		enemies = {},
		stars = {},
		shots = {},
	}
	setmetatable(obj, self)
	self.__index = self
	
	return obj
end

local healthBorder = jar.Shape.Rectangle(6, 1, 50, 10, jar.Color(0, 0, 0, 0), 1, jar.Color.White)
local healthBar = jar.Shape.Rectangle(0, 0, 50, 10, jar.Color.Red, 0, jar.Color.White)
healthBar:SetPosition(6, 1)
local weaponSprite = jar.Sprite()
weaponSprite:SetPosition(60, 2)
weaponSprite:SetScale(2, 2)
--this might not belong in a gamefield - so what?
function Gamefield:RenderGUI()
	--scale must be strictly positive, I'm told.
	healthBar:SetScale(math.max(self.player.health, 1)/self.player.maxHealth, 1)
	weaponSprite:SetImage(self.player.availableWeapons[self.player.currentWeapon].image, true)
	g_Window:Draw(healthBorder)
	g_Window:Draw(healthBar)
	g_Window:Draw(weaponSprite)
end

function Gamefield:RenderGamefield()
	-- remember: our view is (0, 0) to (200, 100)
	for i, star in ipairs(self.stars) do
		star:Render()
	end
	
	for i, shot in ipairs(self.shots) do
		shot:Render()
	end
	
	self.player:Render()
end

function Gamefield:OnKeyDown(key)
	return self.player:OnKeyDown(key)
end

function Gamefield:Update(deltaT)
	self.nextStarIn = self.nextStarIn - deltaT
	if self.nextStarIn <= 0 then
		table.insert(self.stars, Star:New())
		self.nextStarIn = math.random(100, 200)
	end
	local i = 1
	while true do
		if i > #self.stars then
			break
		end
		self.stars[i]:Update(deltaT)
		if self.stars[i]:IsDead() then
			table.remove(self.stars, i)
		else
			i = i + 1
		end
	end
	
	local i = 1
	while true do
		if i > #self.shots then
			break
		end
		local shot = self.shots[i]
		shot:Update(deltaT)
		if shot.dir > 0 then
			for i, enemy in ipairs(self.enemies) do
				if shot:CheckCollision(enemy.sprite) then
					shot:Hit()
					enemy:Damage(shot.damage)
					--todo: explosion!
					break
				end
			end
		else
			if shot:CheckCollision(self.player.sprite) then
				self.player:Damage(shot.damage)
				shot:Hit()
			end
		end
		if shot.dead then
			table.remove(self.shots, i)
		else
			i = i + 1
		end
	end
	--update player and enemies after shots, as they might've been killed!
	self.player:Update(deltaT)
	
	if self.player.health <= 0 then
		GameState.GameOver = true
	end
	
	local i = 1
	while true do
		if i > #self.enemies then
			break
		end
		local enemy = self.enemies[i]
		enemy:Update(deltaT)
		if enemy.health <= 0 then
			table.remove(self.enemies, i)
		else
			i = i + 1
		end
	end
end