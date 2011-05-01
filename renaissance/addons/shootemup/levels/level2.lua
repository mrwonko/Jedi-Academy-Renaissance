return {
	-- time - enemies to spawn at 0 milliseconds in
	--[[
	[0] = {
		{
			type = "enemy", --default
			class = EnemyKamikaze,
			parameters = { position = jar.Vector2f(205, 50), },
		},
	},
	--]]
	[1000] = {
		{
			class = EnemyKamikaze,
			parameters = {	position = jar.Vector2f(205, 50), },
		},
	},
	[3000] = {
		{
			class = EnemyKamikaze,
			parameters = {	position = jar.Vector2f(205, 30), },
		},
	},
	[3500] = {
		{
			class = EnemyKamikaze,
			parameters = {	position = jar.Vector2f(205, 40), },
		},
	},
	[4000] = {
		{
			class = EnemyKamikaze,
			parameters = {	position = jar.Vector2f(205, 50), },
		},
	},
	[4500] = {
		{
			class = EnemyKamikaze,
			parameters = {	position = jar.Vector2f(205, 60), },
		},
	},
	[5000] = {
		{
			class = EnemyKamikaze,
			parameters = {	position = jar.Vector2f(205, 70), },
		},
	},
}