function CheckString(param, paramname, funcname)
	if type(param) ~= "string" then error(funcname .." called with nonstring parameter " .. paramname, 3) end
end
function CheckFunction(param, paramname, funcname)
	if type(param) ~= "function" then error(funcname .." called with nonfunction parameter " .. paramname, 3) end
end
function CheckNumber(param, paramname, funcname)
	if type(param) ~= "number" then error(funcname .." called with nonnumber parameter " .. paramname, 3) end
end
function CheckBool(param, paramname, funcname)
	if type(param) ~= "boolean" then error(funcname .." called with nonboolean parameter " .. paramname, 3) end
end
function CheckEuler(param, paramname, funcname)
	if getmetatable(param) ~= TestEuler then error(funcname .." called with non-Euler parameter " .. paramname, 3) end
end
function CheckVector(param, paramname, funcname)
	if getmetatable(param) ~= TestVector then error(funcname .." called with non-Vector parameter " .. paramname, 3) end
end
function CheckColor(param, paramname, funcname)
	if getmetatable(param) ~= TestColor then error(funcname .." called with non-Color parameter " .. paramname, 3) end
end
function CheckNumParam(parameters, funcname)
	if #parameters > 0 then error(funcname .. " called with too many arguments", 3) end
end