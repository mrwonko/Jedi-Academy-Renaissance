Expression =
{
	Type =
	{
		Constant = 0,
		Variable = 1,
		Add = 2,
		Sub = 3,
		Mul = 4,
		Div = 5,
	},
}

local validTypes =
{
	[Expression.Type.Constant] = true,
	[Expression.Type.Variable] = true,
	[Expression.Type.Add] = true,
	[Expression.Type.Sub] = true,
	[Expression.Type.Mul] = true,
	[Expression.Type.Div] = true,
}

local singleOp =
{
	[Expression.Type.Constant] = true,
	[Expression.Type.Variable] = true,
}

local function IsExpression(val)
	return getmetatable(val) == Expression
end

local function RaiseExpReadOnlyError()
	error("Expressions are readonly!")
end

function Expression:New(expType, lhs, rhs)
	-- create object (readonly)
	self.__index = self
	local obj = setmetatable({}, self)
	
	-- verify expType is valid
	if not validTypes[expType] then
		error("Invalid expression type", 2)
	end
	obj.type = expType
	
	-- verify lhs is valid
	if expType == Expression.Type.Variable then
		if type(lhs) ~= "string" then
			error("Invalid lhs parameter, must be string!", 2)
		end
		-- else: good
	elseif expType == Expression.Type.Constant and type(lhs) ~= "number" then
		error("Invalid lhs parameter, must be number!", 2)
	elseif type(lhs) == "number" then
		if expType ~= Expression.Type.Constant then
			lhs = Expression:New(Expression.Type.Constant, lhs)
		end
		-- else: good
	elseif not IsExpression(lhs) then
		error("Invalid lhs parameter, must be Expression or number!", 2)
	end
	obj.lhs = lhs
	
	-- two operands?
	if not singleOp[expType] then
		-- then verify rhs is valid, too
		if type(rhs) == "number" then
			rhs = Expression:New(Expression.Type.Constant, rhs)
		elseif not IsExpression(rhs) then
			error("Invalid rhs parameter, must be Expression or number!", 2)
		end
	end
	obj.rhs = rhs
	
	-- return object
	return obj
end

-- variables is a table containing the variable values
function Expression:Evaluate(variables)
	if self.type == Expression.Type.Constant then
		return self.lhs
	elseif self.type == Expression.Type.Variable then
		return variables[self.lhs]
	elseif self.type == Expression.Type.Add then
		return self.lhs:Evaluate(variables) + self.rhs:Evaluate(variables)
	elseif self.type == Expression.Type.Sub then
		return self.lhs:Evaluate(variables) - self.rhs:Evaluate(variables)
	elseif self.type == Expression.Type.Mul then
		return self.lhs:Evaluate(variables) * self.rhs:Evaluate(variables)
	elseif self.type == Expression.Type.Div then
		return self.lhs:Evaluate(variables) / self.rhs:Evaluate(variables)
	else
		error("Internal error - invalid Expression got created!")
	end
end

function Expression.__add(lhs, rhs)
	return Expression:New(Expression.Type.Add, lhs, rhs)
end

function Expression.__sub(lhs, rhs)
	return Expression:New(Expression.Type.Sub, lhs, rhs)
end

function Expression.__mul(lhs, rhs)
	return Expression:New(Expression.Type.Mul, lhs, rhs)
end

function Expression.__div(lhs, rhs)
	return Expression:New(Expression.Type.Div, lhs, rhs)
end

-- debug output of the expression
function Expression:ToString(variables)
	if self.type == Expression.Type.Constant then
		return self.lhs .. ""
	elseif self.type == Expression.Type.Variable then
		return self.lhs .. "(=" .. variables[self.lhs] .. ")"
	elseif self.type == Expression.Type.Add then
		return self.lhs:ToString(variables) .. " + " .. self.rhs:ToString(variables)
	elseif self.type == Expression.Type.Sub then
		return self.lhs:ToString(variables) .. " - " .. self.rhs:ToString(variables)
	elseif self.type == Expression.Type.Mul then
		return self.lhs:ToString(variables) .. " * " .. self.rhs:ToString(variables)
	elseif self.type == Expression.Type.Div then
		return self.lhs:ToString(variables) .. " / " .. self.rhs:ToString(variables)
	else
		error("Internal error - invalid Expression got created!")
	end
end