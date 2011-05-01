require("Player.lua")
require("Star.lua")

require("EnemyKamikaze.lua")

local function CheckCollision(sprite1, sprite2)
	local ownsize = sprite1:GetSize()
	local ownorigin = jar.Vector2f(sprite1:GetOrigin()) --copy since we want to change it!
	local ownposition = sprite1:GetPosition()
	ownorigin.x = ownorigin.x * sprite1:GetScale().x
	ownorigin.y = ownorigin.y * sprite1:GetScale().y
	local othersize = sprite2:GetSize()
	local otherorigin = jar.Vector2f(sprite2:GetOrigin())
	otherorigin.x = otherorigin.x * sprite2:GetScale().x
	otherorigin.y = otherorigin.y * sprite2:GetScale().y
	local otherposition = sprite2:GetPosition()
	
	local ownRect = jar.FloatRect(ownposition.x-ownorigin.x, ownposition.y-ownorigin.y, ownsize.x, ownsize.y)
	--the -2 is so they collide a little
	local otherRect = jar.FloatRect(otherposition.x-otherorigin.x+2, otherposition.y-otherorigin.y, othersize.x-2, othersize.y)
	
	return ownRect:Intersects(otherRect)
end

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
		leveltime = 0,
		curlevel = 1,
		levels = {},
		score = 0,
		managedSounds = {},
	}
	setmetatable(obj, self)
	self.__index = self
	
	local i = 1
	while true do
		local f, err = loadfile("levels/level".. i ..".lua")
		if not f then
			jar.Logger.GetDefaultLogger():Info(err, 1)
			break
		end
		table.insert(obj.levels, f())
		i = i + 1
	end
	assert(#obj.levels >= 1)
	
	return obj
end

---- HUD Elements ----

--health
local healthBorder = jar.Shape.Rectangle(6, 1, 50, 10, jar.Color(0, 0, 0, 0), 1, jar.Color.White)
local healthBar = jar.Shape.Rectangle(0, 0, 50, 10, jar.Color.Red, 0, jar.Color.White)
healthBar:SetPosition(6, 1)
--weapon
local weaponSprite = jar.Sprite()
weaponSprite:SetPosition(60, 2)
weaponSprite:SetScale(2, 2)
--score
local scoreString = jar.Text(g_FontManager:GetFont("arial"))
scoreString:SetPosition(100, 0)
--level
local levelString = jar.Text(g_FontManager:GetFont("arial"))
levelString:SetPosition(200, 0)

--this might not belong in a gamefield - so what?
function Gamefield:RenderGUI()
	--scale must be strictly positive, I'm told.
	healthBar:SetScale(math.max(self.player.health, 1)/self.player.maxHealth, 1)
	weaponSprite:SetImage(self.player.availableWeapons[self.player.currentWeapon].image, true)
	scoreString:SetText("score: "  .. self.score)
	-- in case the score string gets too long :P
	while scoreString:GetWidth() + scoreString:GetPosition().x > levelString:GetPosition().x do
		levelString:Move(10, 0)
	end
	levelString:SetText("level: "  .. self.curlevel)
	g_Window:Draw(healthBorder)
	g_Window:Draw(healthBar)
	g_Window:Draw(weaponSprite)
	g_Window:Draw(scoreString)
	g_Window:Draw(levelString)
end

function Gamefield:RenderGamefield()
	-- remember: our view is (0, 0) to (200, 100)
	for _, star in ipairs(self.stars) do
		star:Render()
	end
	
	for _, shot in ipairs(self.shots) do
		shot:Render()
	end
	
	for _, enemy in ipairs(self.enemies) do
		enemy:Render()
	end
	
	self.player:Render()
end

function Gamefield:OnKeyDown(key)
	if key == jar.Key.Back then
		EnemyKamikaze:New{ position = jar.Vector2f(205, 50) }
	end
	return self.player:OnKeyDown(key)
end

function Gamefield:Update(deltaT)
	self.leveltime = self.leveltime + deltaT
	
	-- level end condition check - all enemies dead, none left in list
	local leveldone = true
	
	--keep track of the enemies we've done so we can delete them
	local spawnedEnemies = {}
	--are there still enemies to be spawned?
	for t, enemiesToSpawn in pairs(self.levels[self.curlevel]) do
		leveldone = false --even if they're spawned this frame they won't be dead this frame - and if they are, it doesn't really matter. It's not like the player could die because of it.
		if t <= self.leveltime then
			--spawn enemies!
			for _, info in ipairs(enemiesToSpawn) do
				if (info.type or "enemy") == "enemy" then
					info.class:New(info.parameters)
				--todo: item spawning!
				end
			end
			table.insert(spawnedEnemies, t)
		end
	end
	
	for _, t in ipairs(spawnedEnemies) do
		self.levels[self.curlevel][t] = nil
	end
	
	-- still enemies alive?
	if #self.enemies > 0 then
		leveldone = false
	end
	
	if leveldone then
		self.leveltime = 0
		if self.curlevel + 1 > #self.levels then
			GameState.GameWon = true
			return
		end
		self.curlevel = self.curlevel+1
	end
	
	local i = 1
	while true do
		if i > #self.managedSounds then
			break
		end
		if self.managedSounds[i]:GetStatus() == jar.Sound.Stopped then
			table.remove(self.managedSounds, i)
		else
			i = i + 1
		end
	end
	
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
				if CheckCollision(shot.sprite, enemy.sprite) then
					shot:Hit()
					enemy:Damage(shot.damage)
					--todo: explosion!
					break
				end
			end
		else
			if CheckCollision(shot.sprite, self.player.sprite) then
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
	
	local i = 1
	while true do
		if i > #self.enemies then
			break
		end
		local enemy = self.enemies[i]
		enemy:Update(deltaT)
		if CheckCollision(enemy.sprite, self.player.sprite) then
			enemy:Damage(99999999) --I hope this kills everything - if it doesn't, it might as well damage the player again.
			self.player:Damage(enemy.crashDamage)
			self.score = self.score - enemy.score -- substract this first to even out the awarded points for enemy's death
		end
		if enemy.health <= 0 then
			self.score = self.score + enemy.score
			table.remove(self.enemies, i)
		elseif enemy.sprite:GetPosition().x < -5 then
			table.remove(self.enemies, i)
		else
			i = i + 1
		end
	end
	
	--last since a lot can kill him
	if self.player.health <= 0 then
		GameState.GameOver = true
	end
end