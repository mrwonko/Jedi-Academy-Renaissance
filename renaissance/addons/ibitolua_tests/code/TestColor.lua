TestColor = {}

function TestColor:New(r, g, b, a)
	if not r or not g or not b or not a then error("Missing parameter for color", 2) end
	local obj = {r = r, g = g, b = b, a = a}
	setmetatable(obj, self)
	self.__index = self
	return obj
end

function vectorAndAlphaToColor(vec, alpha, ...)
	if #{...} > 0 then error("Too many parameters to vectorAndAlphaToColor()", 2) end
	if getmetatable(vec) ~= TestVector then error("vectorAndAlphaToColor: first parameter no vector!", 2) end
	if type(alpha) ~= "number" then error("vectorAndAlphaToColor: second parameter no number!", 2) end
	return TestColor:New(vec.x, vec.y, vec.z, alpha)
end