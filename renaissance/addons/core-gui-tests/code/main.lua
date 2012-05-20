local metatable =
{
	__add = function(lhs, rhs)
		print(lhs, rhs)
	end
}

local obj = setmetatable({}, metatable)
print(obj + 3)
print(5 + obj)