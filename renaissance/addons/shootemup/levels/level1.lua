return {
	-- time - enemies to spawn at 0 milliseconds in
	[0] = {
		{
			type = "enemy", --default
			class = EnemyKamikaze,
			parameters = { position = jar.Vector2f(205, 50), },
		},
	},
	[1000] = {
		{
			class = EnemyKamikaze,
			position = 50,
			parameters = { position = jar.Vector2f(205, 50), },
		},
	},
	[2000] = {
		{
			class = EnemyKamikaze,
			position = 50,
			parameters = { position = jar.Vector2f(205, 50), },
		},
	},
}