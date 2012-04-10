-- setup
local addInaccesible = false -- if the key to a table is a function/table/uservalue, it cannot be indexed (well) - should those still be added?


print("Luabind documentation generation")

-- walk through the globals, following every table, finding luabind generated stuff, remembering how to get there
local todo = {[_G] = ""}
-- remember what you already processed in case of cycles
local processed = {[_G] = true}

local writeableTypes = { string = true, number = true, boolean = true }
local unaryOperators = { __unm = true, __len = true }
local unaryOperatorSymbols = { __unm = "-", __len = "#" }
local binaryOperators = { __add = true, __sub = true, __mul = true, __div = true, __mul = true, __mod = true, __pow = true, __concat = true, __lt = true, __le = true, __eq = true }
local binaryOperatorSymbols = { __add = "+", __sub = "-", __mul = "*", __div = "/", __mul = "/", __mod = "%", __pow = "^", __concat = "..", __lt = "<", __le = "<=", __eq = "==" }

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

local unambiguousLuabindClassesByName = {}
local luabindClassesByActualName = {}

for name, classes in pairs(luabindClassesByName) do
	if #classes == 1 then -- don't replace ambiguous names
		unambiguousLuabindClassesByName[name] = classes[1]
	end
	for _, class in ipairs(classes) do
		luabindClassesByActualName[class.actualName] = class
	end
end

for _, class in ipairs(unnamedLuabindClasses) do
	luabindClassesByActualName[class.actualName] = class
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

-- replaces the internal names with the actual ones, optionally formatting them
local function processSignature(signature, functionInfo, objectFormatter)
	-- split into return type(, name) and parameters
	retVals, parameters = signature:match(string.format("^(.*)%s(%%(.+)$", functionInfo.name))
	if not retVals then
		error("Signature does not match the pattern!")
	end
	
	-- remove optional lua_State* parameter(s?)
	parameters = parameters:gsub("([^%%a%%._])lua_State%*,?([^%%a_])", "%1%2")
	
	--   replace all types with their "actual" name, if that is unambiguous
	
	-- prefix return value(s) with a space (which will later be removed) so I can replace only whole words
	retVals = " " .. retVals
	
	for name, class in pairs(unambiguousLuabindClassesByName) do
		local pattern = "([^%.%a%d_])"..name.."([^%.%a%d_])"
		local replace = "%1" .. escapePercents(objectFormatter(class.actualName, class)) .. "%2"
		retVals = retVals:gsub(pattern, replace)
		parameters = parameters:gsub(pattern, replace)
	end
	
	-- remove prefixed space
	assert(retVals:sub(1, 1) == " ")
	retVals = retVals:sub(2)
	
	return string.format("%s%s%s", retVals, functionInfo.actualName, parameters)
end

-- delete tables
todo = nil
processed = nil

-- sort by name
-- table.sort(luabindObjects, function(lhs, rhs) return lhs.name < rhs.name end)

-- function to format object names (e.g. jar.Vector2f -> <a href="jar/Vector2f.html")
local function objectFormatter(text, classInfo)
	return text
end

local function objectFormatter(text, classInfo)
	return "<class>" .. text .. "</class>"
end

-- preprocess function signatures
for _, overloads in pairs(luabindFunctionsByActualName) do
	for _, info in ipairs(overloads) do
		assert(info.type == "function")
		info.processedSignature = processSignature(info.signature, info, objectFormatter)
	end
end

-- preprocess methods, separating constructors and moving overloads into separate copies
local function processMethods(class)
	local methods = {} -- can't overwrite/change during iteration
	class.constructors = {}
	class.unaryOperators = {}
	class.binaryOperators = {}
	for _, method in ipairs(class.methods) do
		for _, signature in ipairs(method.overloads) do
			local newMethod = {}
			-- copy old function info
			for k, v in pairs(method) do
				if k ~= "overloads" then
					newMethod[k] = v
				end
			end
			assert(newMethod.signature == nil)
			assert(newMethod.actualName == nil)
			-- different handling for constructors, operators and "normal" member functions
			
			-- constructors
			if newMethod.name == "__init" then
				-- remove the luabind::argument part of the signature
				local replaced
				newMethod.signature, replaced = signature:gsub("void __init%(luabind::argument const&,?", "__init(")
				assert(replaced == 1)
				newMethod.actualName = class.actualName
				-- process the signature
				newMethod.processedSignature = processSignature(newMethod.signature, newMethod, objectFormatter)
				table.insert(class.constructors, newMethod)
			
			-- unary operators - #obj and -obj
			elseif unaryOperators[newMethod.name] then
				newMethod.actualName = newMethod.name
				local sig = processSignature(signature, newMethod, function (text, _) return text end) --no formatting yet
				
				local returnType, arg = sig:match("^(.+) " .. newMethod.name .. "%(([^%)]+)%)")
				assert(returnType and arg)
				-- beautify
				local className, suffix = arg:match("^([_%a][_%a%d%.]+)(.*)$")
				local argClass = luabindClassesByActualName[className]
				if argClass then
					arg = objectFormatter(className, argClass) .. suffix
				end
				table.insert(class.unaryOperators, {name = newMethod.name, argument = arg, returnType = returnType})
			-- binary operators
			elseif binaryOperators[newMethod.name] then
				
				newMethod.actualName = newMethod.name
				local sig = processSignature(signature, newMethod, function (text, _) return text end) --no formatting yet
				
				local arg1, arg2 = sig:match("^.+ " .. newMethod.name .. "%(([^,]+),([^%)]+)%)") 
				assert(arg1 and arg2)
				local args = {arg1, arg2}
				-- beautify
				for i = 1, 2 do
					local className, suffix = args[i]:match("^([_%a][_%a%d%.]+)(.*)$")
					local argClass = luabindClassesByActualName[className]
					if argClass then
						args[i] = objectFormatter(className, argClass) .. suffix
					end
				end
				table.insert(class.binaryOperators, {name = newMethod.name, arguments = args})
			-- "ordinary" member functions
			else
				-- remove "self" first argument (attention: is either this class or one of its bases!)
				local pattern = "%(([%a%d%%._]+)[&%*],?"
				match = signature:match(pattern)
				if match == nil then
					pattern = "%(([%a%d%%._]+) const[&%*],?"
					match = signature:match(pattern)
				end
				if not match then
					print(signature)
				end
				assert(match)
				if match ~= class.name and match ~= "void" then -- some libraries cast down to void* to the data opaque
					--todo: check base classes
					--print(("first member function parameter is %s, not %s - this is probably due to inheritance, add that."):format(match, class.name))
				end
				newMethod.signature = signature:gsub(pattern, "(")
				newMethod.actualName = newMethod.name
				-- process the signature
				newMethod.processedSignature = processSignature(newMethod.signature, newMethod, objectFormatter)
				table.insert(methods, newMethod)
			end
		end
	end
	class.methods = methods
end

for _, t in pairs(luabindClassesByName) do
	for _, info in ipairs(t) do
		processMethods(info)
	end
end

for _, info in ipairs(unnamedLuabindClasses) do
	processMethods(info)
end

-- todo: generate documentation

print()
print("== Classes == ")
print()

local function PrintClassInfo(info)
	assert(info.type == "class")
	print(("= %s ="):format(info.actualName))
	print()
	
	-- print constructors, if any
	if #info.constructors > 0 then
		print("- constructors -")
		for _, funcInfo in ipairs(info.constructors) do
			print(funcInfo.processedSignature)
		end
		print()
	end
	
	-- print methods, if any
	if #info.methods > 0 then
		print("- methods -")
		for _, funcInfo in ipairs(info.methods) do
			print(funcInfo.processedSignature)
		end
		print()
	end
	
	-- print properties, if any
	if #info.properties > 0 then
		print("- properties -")
		for _, propInfo in ipairs(info.properties) do
			print(propInfo.name)
		end
		print()
	end
	
	-- print constants, if any
	if #info.constants > 0 then
		-- sort constants by value (as opposed to by name, the default order)
		table.sort(info.constants, function(lhs, rhs) return lhs.value < rhs.value end)
		print("- constants -")
		for _, constantInfo in ipairs(info.constants) do
			print(("%s (%i)"):format(constantInfo.name, constantInfo.value))
		end
		print()
	end
	
	-- print operators, if any
	if #info.unaryOperators > 0 then
		print("- unary operators -")
		for _, op in ipairs(info.unaryOperators) do
			print(op.returnType .. " " .. unaryOperatorSymbols[op.name] .. " " .. op.argument)
		end
		print()
	end
	if #info.binaryOperators > 0 then
		print("- binary operators -")
		for _, op in ipairs(info.binaryOperators) do
			print(op.arguments[1] .. " " .. binaryOperatorSymbols[op.name] .. " " .. op.arguments[2])
		end
		print()
	end
end

for _, t in pairs(luabindClassesByName) do
	for _, info in ipairs(t) do
		PrintClassInfo(info)
	end
end

for _, info in ipairs(unnamedLuabindClasses) do
	PrintClassInfo(info)
end

print()
print("== Functions ==")
print()
for _, overloads in pairs(luabindFunctionsByActualName) do
	for _, info in ipairs(overloads) do
		assert(info.type == "function")
		print(info.processedSignature)
	end
end

print()