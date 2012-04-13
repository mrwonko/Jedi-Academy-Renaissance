TestVector = {}
function TestVector:Equals(rhs)
	if getmetatable(rhs) ~= TestVector then error("Trying to compare Vector and Non-Vector", 2) end
	return (lhs.X == rhs.X) and (lhs.Y == rhs.Y) and (lhs.Z == rhs.Z)
end

function TestVector:New(x, y, z)
	if type(x) ~= "number" or type(y) ~= "number" or type(z) ~= "number" then
		error("Invalid parameter to vector()", 3) -- called by vector() function
	end
	local obj = {X = x, Y = y, Z = z}
	setmetatable(obj, self)
	self.__index = self
	self.__eq = TestVector.Equals
	return obj
end