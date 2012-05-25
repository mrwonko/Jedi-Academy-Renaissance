Layout = {}

function Layout:New()
	self.__index = self
	local obj = setmetatable({}, self)
	return obj
end

-- sets the properties from a given table
-- raises an error when something crucial is missing
function Layout:FromTable(t)
	if type(t.name) ~= "string" then error("No name string supplied!") end
	self.name = t.name
end