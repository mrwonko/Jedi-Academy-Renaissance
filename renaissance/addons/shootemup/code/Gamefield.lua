require("Player.lua")
require("Star.lua")
require("Particle.lua")

--for loading: all enemy types must be compiled!
require("EnemyKamikaze.lua")
require("EnemyAsteroid.lua")
require("EnemyShoot1.lua")
require("EnemyShoot2.lua")
require("EnemyBoss.lua")

local function CheckCollision(sprite1, sprite2)
	local bounds1 = sprite1:GetGlobalBounds()
	local bounds2 = sprite2:GetGlobalBounds()
	return bounds1:Intersects(bounds2)
	
	--local ownRect = jar.FloatRect(ownposition.X-ownorigin.X, ownposition.Y-ownorigin.Y, ownsize.X, ownsize.Y)
	--the -2 is so they collide a little
	-- later me: wtf do I mean by that?
	--local otherRect = jar.FloatRect(otherposition.X-otherorigin.X+2, otherposition.Y-otherorigin.Y, othersize.X-2, othersize.Y)
	--return ownRect:Intersects(otherRect)
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
	}
	setmetatable(obj, self)
	self.__index = self
	
	local i = 1
	while true do
		local f, err = loadfile("levels/level".. i ..".lua")
		if not f then
			jar.Logger.GetDefaultLogger():Log(err)
			break
		end
		table.insert(obj.levels, f())
		i = i + 1
	end
	assert(#obj.levels >= 1)
	
	ParticleManager.particles = {}
	
	return obj
end

---- HUD Elements ----

--health
local healthBorder = jar.RectangleShape(jar.Vector2f(50, 10))
healthBorder:SetFillColor(jar.Color.Transparent)
healthBorder:SetPosition(6, 1)
healthBorder:SetOutlineThickness(1)
healthBorder:SetOutlineColor(jar.Color.White)
local healthBar = jar.RectangleShape(jar.Vector2f(50, 10))
healthBar:SetFillColor(jar.Color.Red)
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
	weaponSprite:SetTexture(self.player.availableWeapons[self.player.currentWeapon].image, true)
	scoreString:SetText("score: "  .. self.score)
	-- in case the score string gets too long :P
	while scoreString:GetWidth() + scoreString:GetPosition().X > levelString:GetPosition().X do
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
	
	ParticleManager:Render()
end

function Gamefield:OnKeyDown(key)
	return self.player:OnKeyDown(key)
end

function Gamefield:Update(deltaT)
	self.leveltime = self.leveltime + deltaT
	
	ParticleManager:Update(deltaT)
	
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
	
	self.nextStarIn = self.nextStarIn - deltaT
	if self.nextStarIn <= 0 then
		--table.insert(self.stars, Star:New())
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
		elseif enemy.sprite:GetPosition().X < -5 then
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