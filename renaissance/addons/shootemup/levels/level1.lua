return {
	[5000] = {
		{
			class = EnemyAsteroid,
			parameters = {	position = jar.Vector2f(90, -5),
							movedir = jar.Vector2f(-0.02, 0.01), },
		},
		{
			class = EnemyAsteroid,
			parameters = {	position = jar.Vector2f(205, 70),
							movedir = jar.Vector2f(-0.025, -0.005),
							size = 12, },
		},
	},
	[6000] = {
		{
			class = EnemyAsteroid,
			parameters = {	position = jar.Vector2f(205, 40),
							movedir = jar.Vector2f(-0.03, 0.002), },
		},
	},
	[6500] = {
		{
			class = EnemyAsteroid,
			parameters = {	position = jar.Vector2f(205, 77),
							movedir = jar.Vector2f(-0.03, -0.002), },
		},
	},
}