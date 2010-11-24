--[[! \brief Completes str by looking at the available strings, optionally caseSensitive (default: not)
--]]
function AutoComplete(str, available, caseSensitive)
	caseSensitive = caseSensitive or false
	if type(str) ~= "string"  then
		jar.Logger.GetDefaultLogger():Warning("AutoComplete() called with invalid argument!")
		return "", {}
	elseif type(available) ~= "table" then
		jar.Logger.GetDefaultLogger():Warning("AutoComplete() called with invalid argument!")
		return "", {}
	end
	if not caseSensitive then
		str = string.lower(str)
	end
	local possibilities = {}
	local len = #str
	-- find names that start like this
	for _, name in ipairs(available) do
		if (caseSensitive and string.sub(name, 1, len) == str) or (not caseSensitive and string.lower(string.sub(name, 1, len)) == str) then
			if caseSensitive then
				table.insert(possibilities, name)
			else
				table.insert(possibilities, string.lower(name))
			end
		end
	end
	-- trivial cases: no matches or 1 match
	if #possibilities == 0 then
		return str, possibilities
	elseif #possibilities == 1 then
		return possibilities[1], possibilities
	end
	-- nontrivial
	
	-- helper function, returns the part that 2 strings have in common
	local function GetCommon(str1, str2)
		local i = 1
		while i <= #str1 and i <= #str2 do
			if string.sub(str1, i, i) ~= string.sub(str2, i, i) then
				return string.sub(str1, 1, i-1)
			end
			i = i + 1
		end
		return string.sub(str1, 1, i)
	end
	local longestPossible = possibilities[1]
	for i = 2, #possibilities do
		longestPossible = GetCommon(longestPossible, possibilities[i])
	end
	return longestPossible, possibilities
end