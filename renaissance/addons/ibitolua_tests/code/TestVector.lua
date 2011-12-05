TestVector = {}
function TestVector:Equals(rhs)
	if getmetatable(rhs) ~= TestVector then error("Trying to compare Vector and Non-Vector", 2) end
	return (lhs.x == rhs.x) and (lhs.y == rhs.y) and (lhs.z == rhs.z)
end

function TestVector:New(x, y, z)
	if type(x) ~= "number" or type(y) ~= "number" or type(z) ~= "number" then
		error("Invalid parameter to vector()", 3) -- called by vector() function
	end
	local obj = {x = x, y = y, z = z}
	setmetatable(obj, self)
	self.__index = self
	self.__eq = TestVector.Equals
	return obj
end