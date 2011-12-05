TestEuler = {}
function TestEuler:Equals(rhs)
	if getmetatable(rhs) ~= TestEuler then error("Trying to compare Euler and Non-Eueler", 2) end
	return (self.pitch == rhs.pitch) and (self.yaw == rhs.yaw) and (self.roll == rhs.roll)
end

function TestEuler:New(pitch, yaw, roll)
	if type(pitch) ~= "number" or type(yaw) ~= "number" or type(roll) ~= "number" then
		error("Invalid parameter to euler()", 3) -- called by euler() function
	end
	local obj = {pitch = pitch, yaw = yaw, roll = roll}
	setmetatable(obj, self)
	self.__index = self
	self.__eq = TestEuler.Equals
	return obj
end