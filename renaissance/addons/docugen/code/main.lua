-- setup
local addInaccesible = false -- if the key to a table is a function/table/uservalue, it cannot be indexed (well) - should those still be added?


print("Luabind documentation generation")

-- walk through the globals, following every table, finding luabind generated stuff, remembering how to get there
local todo = {[_G] = ""}
-- remember what you already processed in case of cycles
local processed = {[_G] = true}

local writeableTypes = { string = true, number = true, boolean = true }

-- for each (internal) name, an array of luabind class info tables (usually just 1, except when names clash - used for improving signatures by replacing these with the actual names)
local luabindClassesByName = {}
-- all the Luabind classes without internal names (no idea how that's possible, but let's assume it is, okay?)
local unnamedLuabindClasses = {}
-- all the Luabind functions by their actual name
local luabindFunctionsByActualName = {}

local done = false
while not done do
	-- get the next entry from the todo list
	local object, name = next(todo)
	
	-- if there is no next entry, we're done.
	if object == nil then
		done = true
	else
		-- remove entry from todo list
		todo[object] = nil
		-- if this is a table, add all its unprocessed entries
		if type(object) == "table" then
			local name = name
			if name == "" then
				name = nil
			end
			for key, value in pairs(object) do
				-- only add unprocessed ones
				if processed[value] == nil then
					local keyType = type(key)
					if writeableTypes[keyType] or addInaccesible then
						assert(todo[value] == nil)
						processed[value] = true
						if keyType == "string" then
							todo[value] = (name and (name .. "." .. key)) or key
						elseif writeableTypes[keyType] then
							todo[value] = (name or "_G") .. "[" .. key .. "]"
						else
							todo[value] = "<inaccesible>"
						end
					end
				end
			end
		else -- not a table
			-- try to get information on this object
			local info = jar.GetLuabindInfo(object)
			-- this only succeeds if it's a luabind function or class.
			if info then
				info.actualName = name
				if info.type == "class" then
					-- no idea if classes without names are even possible here, but if they are, they are put in a different table
					if info.name then
						-- I create a table for each (simple) name in case of nameclashes (since luabind ignores namespaces here)
						if luabindClassesByName[info.name] == nil then
							luabindClassesByName[info.name] = {}
						else
							local msg
							if #luabindClassesByName[info.name] == 1 then
								print(info.name .. " is ambiguous" .. ", used by " .. luabindClassesByName[info.name][1].actualName .. " and " .. info.actualName)
							else
								print(info.name .. " also used by " .. info.actualName)
							end
						end
						table.insert(luabindClassesByName[info.name], info)
					else
						table.insert(unnamedLuabindClasses, info)
					end
				else
					assert(info.type == "function")
					-- the same actual name shouldn't exist more than once
					assert(luabindFunctionsByActualName[info.actualName] == nil)
					-- add all overloads
					luabindFunctionsByActualName[info.actualName] = {}
					for _, signature in ipairs(info.overloads) do
						table.insert(luabindFunctionsByActualName[info.actualName], {
							name = info.name, -- for searching in the signature
							actualName = info.actualName, -- for replacing in the signature
							signature = signature,
							type = info.type,
						})
					end
				end
			end
		end
	end
end

-- puts a % before all % in the string to escape for use in patterns
local function escapePercents(s)
	local parts = {}
	while true do
		local index = s:find("%%")
		if index == nil then
			parts[#parts + 1] = s
			break
		end
		parts[#parts + 1] = s:sub(1, index)
		parts[#parts + 1] = "%"
		s = s:sub(index + 1)
	end
	return table.concat(parts)
end

-- replaces the internal names with the actual ones, optionally formatting them (except for the function name itself)
local function processSignature(signature, objectInfo, objectFormatter)
	-- formatting to apply to the types (e.g. linkify them)
	objectFormatter = objectFormatter or function(info) return info.actualName end
	-- split into return type(, name) and parameters
	retVals, parameters = signature:match(string.format("^(.+)%s(%%(.+)$", objectInfo.name))
	if not retVals then
		error("Signature does not match the pattern!")
	end
	--   replace all types with their "actual" name, if that is unambiguous
	
	-- prefix return value(s) with a space (which will later be removed) so I can replace only whole words
	retVals = " " .. retVals
	
	for name, classes in pairs(luabindClassesByName) do
		if #classes == 1 then -- don't replace ambiguous names
			local pattern = string.format("([^%%.%%a])%s([^%%.%%a])", name)
			local replace = "%1" .. escapePercents(objectFormatter(classes[1])) .. "%2"
			retVals = retVals:gsub(pattern, replace)
			parameters = parameters:gsub(pattern, replace)
		end
	end
	
	-- remove prefixed space
	assert(retVals:sub(1, 1) == " ")
	retVals = retVals:sub(2)
	
	return string.format("%s%s%s", retVals, objectInfo.actualName, parameters)
end

-- delete tables
todo = nil
processed = nil

-- sort by name
-- table.sort(luabindObjects, function(lhs, rhs) return lhs.name < rhs.name end)

-- todo: generate documentation

print()
print("== Classes == ")
print()
for _, t in pairs(luabindClassesByName) do
	for _, info in ipairs(t) do
		assert(info.type == "class")
		print(info.actualName)
	end
end

for _, info in ipairs(unnamedLuabindClasses) do
	if info.type == "function" then
		--print(info.signature)
		print(processSignature(info.signature, info, function(info) return info.name end))
	else
		print(info.name)
	end
end

print()
print("== Functions ==")
print()
for _, overloads in pairs(luabindFunctionsByActualName) do
	for _, info in ipairs(overloads) do
		assert(info.type == "function")
		print(processSignature(info.signature, info))
	end
end

print()