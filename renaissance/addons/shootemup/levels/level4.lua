local ret = {}

for i = 1, 9 do
	ret[i*1000] = { {
		class = EnemyShoot1,
		parameters = {	position = jar.Vector2f(205, i*10), },
	}}
end

return ret