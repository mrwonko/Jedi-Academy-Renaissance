--info about ibi files
local icarusInfo = {}
icarusInfo.ident = "IBI\0" -- \x[xx] is in decimal
icarusInfo.version = "\195\245\200\63"
icarusInfo.defines =
{
	--token defines
	TK_EOF = -1,
	TK_UNDEFINED = 0,
	TK_COMMENT = 1,
	TK_EOL = 2,
	TK_CHAR = 3,
	TK_STRING = 4,
	TK_INT = 5,
	TK_INTEGER = TK_INT,
	TK_FLOAT = 6,
	TK_IDENTIFIER = 7,
	TK_USERDEF = 8,

	TK_BLOCK_START = 8,
	TK_BLOCK_END = 9,
	TK_VECTOR_START = 10,
	TK_VECTOR_END = 11,
	TK_OPEN_PARENTHESIS = 12,
	TK_CLOSED_PARENTHESIS = 13,
	TK_VECTOR = 14,
	TK_GREATER_THAN = 15,
	TK_LESS_THAN = 16,
	TK_EQUALS = 17,
	TK_NOT = 18,
	NUM_USER_TOKENS = 19,

	--ID defines
	ID_AFFECT = 19,
	ID_SOUND = 20,
	ID_MOVE = 21,
	ID_ROTATE = 22,
	ID_WAIT = 23,
	ID_BLOCK_START = 24,
	ID_BLOCK_END = 25,
	ID_SET = 26,
	ID_LOOP = 27,
	ID_LOOPEND = 28,
	ID_PRINT = 29,
	ID_USE = 30,
	ID_FLUSH = 31,
	ID_RUN = 32,
	ID_KILL = 33,
	ID_REMOVE = 34,
	ID_CAMERA = 35,
	ID_GET = 36,
	ID_RANDOM = 37,
	ID_IF = 38,
	ID_ELSE = 39,
	ID_REM = 40,
	ID_TASK = 41,
	ID_DO = 42,
	ID_DECLARE = 43,
	ID_FREE = 44,
	ID_DOWAIT = 45,
	ID_SIGNAL = 46,
	ID_WAITSIGNAL = 47,
	ID_PLAY = 48,

	ID_TAG = 49,
	ID_EOF = 50,
	NUM_IDS = 51,

	--Type defines
	--Wait types
	TYPE_WAIT_COMPLETE = 51,
	TYPE_WAIT_TRIGGERED = 52,

	--Set types
	TYPE_ANGLES = 53,
	TYPE_ORIGIN = 54,

	--Affect types
	TYPE_INSERT = 55,
	TYPE_FLUSH = 56,

	--Camera types
	TYPE_PAN = 57,
	TYPE_ZOOM = 58,
	TYPE_MOVE = 59,
	TYPE_FADE = 60,
	TYPE_PATH = 61,
	TYPE_ENABLE = 62,
	TYPE_DISABLE = 63,
	TYPE_SHAKE = 64,
	TYPE_ROLL = 65,
	TYPE_TRACK = 66,
	TYPE_DISTANCE = 67,
	TYPE_FOLLOW = 68,

	--Variable type
	TYPE_VARIABLE = 69,

	TYPE_EOF = 70,
	NUM_TYPES = 71,
}
	
icarusInfo.IDLookup =
{
	[-1] = "TK_EOF",
	[0] = "TK_UNDEFINED",
	[1] = "TK_COMMENT",
	[2] = "TK_EOL",
	[3] = "TK_CHAR",
	[4] = "TK_STRING",
	[5] = "TK_INT",
	[6] = "TK_FLOAT",
	[7] = "TK_IDENTIFIER",
	[8] = "TK_BLOCK_START",
	[9] = "TK_BLOCK_END",
	[10] = "TK_VECTOR_START",
	[11] = "TK_VECTOR_END",
	[12] = "TK_OPEN_PARENTHESIS",
	[13] = "TK_CLOSED_PARENTHESIS",
	[14] = "TK_VECTOR",
	[15] = "TK_GREATER_THAN",
	[16] = "TK_LESS_THAN",
	[17] = "TK_EQUALS",
	[18] = "TK_NOT",
	[19] = "ID_AFFECT",
	[20] = "ID_SOUND",
	[21] = "ID_MOVE",
	[22] = "ID_ROTATE",
	[23] = "ID_WAIT",
	[24] = "ID_BLOCK_START",
	[25] = "ID_BLOCK_END",
	[26] = "ID_SET",
	[27] = "ID_LOOP",
	[28] = "ID_LOOPEND",
	[29] = "ID_PRINT",
	[30] = "ID_USE",
	[31] = "ID_FLUSH",
	[32] = "ID_RUN",
	[33] = "ID_KILL",
	[34] = "ID_REMOVE",
	[35] = "ID_CAMERA",
	[36] = "ID_GET",
	[37] = "ID_RANDOM",
	[38] = "ID_IF",
	[39] = "ID_ELSE",
	[40] = "ID_REM",
	[41] = "ID_TASK",
	[42] = "ID_DO",
	[43] = "ID_DECLARE",
	[44] = "ID_FREE",
	[45] = "ID_DOWAIT",
	[46] = "ID_SIGNAL",
	[47] = "ID_WAITSIGNAL",
	[48] = "ID_PLAY",
	[49] = "ID_TAG",
	[50] = "ID_EOF",
	[51] = "TYPE_WAIT_COMPLETE",
	[52] = "TYPE_WAIT_TRIGGERED",
	[53] = "TYPE_ANGLES",
	[54] = "TYPE_ORIGIN",
	[55] = "TYPE_INSERT",
	[56] = "TYPE_FLUSH",
	[57] = "TYPE_PAN",
	[58] = "TYPE_ZOOM",
	[59] = "TYPE_MOVE",
	[60] = "TYPE_FADE",
	[61] = "TYPE_PATH",
	[62] = "TYPE_ENABLE",
	[63] = "TYPE_DISABLE",
	[64] = "TYPE_SHAKE",
	[65] = "TYPE_ROLL",
	[66] = "TYPE_TRACK",
	[67] = "TYPE_DISTANCE",
	[68] = "TYPE_FOLLOW",
	[69] = "TYPE_VARIABLE",
	[70] = "TYPE_EOF",
	[71] = "NUM_TYPES",
}
-- I don't want to type that much
local def = icarusInfo.defines

-- table containing all compound statements for fast checks whether children need to be loaded
local compoundStatements =
{
	[icarusInfo.defines.ID_IF] = true,
	[icarusInfo.defines.ID_LOOP] = true,
	[icarusInfo.defines.ID_ELSE] = true,
	[icarusInfo.defines.ID_AFFECT] = true,
	[icarusInfo.defines.ID_TASK] = true,
}

-- members that contain no useful information (that I know of)
local simpleMembers =
{
	[icarusInfo.defines.TK_VECTOR] = true,
	[icarusInfo.defines.TK_GREATER_THAN] = true,
	[icarusInfo.defines.TK_LESS_THAN] = true,
	[icarusInfo.defines.TK_EQUALS] = true,
	[icarusInfo.defines.TK_NOT] = true,
	[icarusInfo.defines.ID_GET] = true,
	[icarusInfo.defines.ID_TAG] = true,
	[icarusInfo.defines.ID_RANDOM] = true,
}

-- names of functions to create

local functionNames =
{
	print = "print",
	randomInt = "randomInt", -- I don't think this even exists - todo: remove if not used
	randomFloat = "randomFloat",
	affect = "affect",
	vector = "vector",
	euler = "euler",
	getEntity = "getEntity",
	task = "task",
	run = "run", --not dofile since this may need to do ibi to lua conversion and file search
	-- entity methods
	get = "Get",
	getAngles = "GetAngles",
	getOrigin = "GetOrigin",
}

-- names of constants to use

local constantNames =
{
	flush = "FLUSH",
	insert = "INSERT",
}

-- second parameter for self:Get (the type) to use

local getTypes =
{
	[def.TK_FLOAT] = "float",
	[def.TK_VECTOR] = "vector",
	[def.TK_STRING] = "string",
}


local function Escape(str)
	-- escape un-escaped backslash at end
	local numBackslashes = 0 -- at the end
	local pos = -1
	while str:sub(pos, pos) == "\\" do
		numBackslashes = numBackslashes + 1
		pos = pos - 1
	end
	if numBackslashes % 2 == 1 then
		str = str .. "\\"
	end
	-- escape newlines
	str = str:gsub("\n", "\\n")
	-- escape un-escaped quotes
	local parts = {}
	local numBackslashes = 0
	for index = 1, #str do
		local char = str:sub(index, index)
		if char == "\\" then
			numBackslashes = numBackslashes + 1
			parts[#parts + 1] = "\\"
		elseif char == "\"" and numBackslashes % 2 == 0 then
			parts[#parts + 1] = "\\\""
			numBackslashes = 0
		elseif char == "\'" and numBackslashes % 2 == 0 then
			parts[#parts + 1] = "\\\'"
			numBackslashes = 0
		else
			parts[#parts + 1] = char
			numBackslashes = 0
		end
	end
	str = table.concat(parts)
	return str
end

-- Checks if this is a valid parameter
-- Gets a table containing the allowed types as keys, and true as value, except:
-- * ID_RANDOM gets type of random - TK_FLOAT or TK_INT
-- * ID_GET gets table of allowed Get types as value - only TK_FLOAT, TK_STRING and TK_VECTOR are possible
-- * TK_VECTOR gets name of function to call (usually vector or euler)
-- allowed: TK_VECTOR, TK_FLOAT, TK_STRING, TK_CHAR, ID_GET (see above), ID_RANDOM (see above), ID_TAG, TK_GREATER_THAN, TK_LESS_THAN, TK_EQUALS, TK_NOT
-- returns false if not allowed, otherwise true, new value of curMember (i.e. increased by number of read members) and the parameter as a string

local ValidateParameter -- forward declaration for recursion - is this necessary?
local function ValidateParameter(members, curMemberIndex, allowed)
	if type(allowed) ~= "table" then
		error("\"allowed\" parameter is no table", 2)
	end
	local curMember = members[curMemberIndex]
	if not curMember then return false end
	curMemberIndex = curMemberIndex + 1
	local memberType = curMember.type
	if not allowed[memberType] then
		return false
	end
	if memberType == def.TK_INT then
		return true, curMemberIndex, string.format("%i", curMember.data)
	elseif memberType == def.TK_FLOAT then
		return true, curMemberIndex, string.format("%f", curMember.data)
	elseif memberType == def.TK_CHAR then
		return true, curMemberIndex, "\"" .. string.char(curMember.data) .. "\""
	elseif memberType == def.TK_STRING then
		return true, curMemberIndex, "\"" .. curMember.data .. "\""
	elseif memberType == def.TK_VECTOR then
		-- vectors are usually 3 floats, though those can be gotten through ID_GET as well (and more?)
		local values = {}
		for i=1,3 do
			local success, code
			success, curMemberIndex, code = ValidateParameter(members, curMemberIndex,
				{
					[def.TK_FLOAT] = true,
					[def.ID_GET] = {[def.TK_FLOAT] = true,},
					[def.ID_RANDOM] = def.TK_FLOAT,
				})
			if not success then
				return false
			end
			values[i] = code
		end
		return true, curMemberIndex, allowed[def.TK_VECTOR] .. "(" .. table.concat(values, ", ") .. ")"
	elseif memberType == def.ID_GET then
		-- are there enough parameters for a get?
		if curMemberIndex + 1 > #members then
			return false
		end
		local getTypeMember = members[curMemberIndex]
		curMemberIndex = curMemberIndex + 1
		if getTypeMember.type ~= def.TK_FLOAT then
			return false
		end
		if not allowed[def.ID_GET][getTypeMember.data] then
			return false
		end
		local success, nameCode
		success, curMemberIndex, nameCode = ValidateParameter(members, curMemberIndex,
			{
				[def.TK_STRING] = true,
				[def.ID_GET] = {[def.TK_STRING] = true},
			})
		if not success then
			return false
		end
		if not getTypes[getTypeMember.data] then
			return false
		end
		return true, curMemberIndex, "self:" .. functionNames.get .. "(" .. nameCode .. ", \"" .. getTypes[getTypeMember.data] .. "\")"
	elseif memberType == def.ID_RANDOM then
		local randomCode = {}
		--todo: is there even another type than float?
		local randomType = allowed[def.ID_RANDOM]
		for i = 1, 2 do
			local success, code
			local temp = curMemberIndex
			success, curMemberIndex, code = ValidateParameter(members, curMemberIndex,
				{
					[randomType] = true,
					[def.ID_GET] = {[randomType] = true,},
					[def.ID_RANDOM] = randomType,
				})
			if not success then
				return false
			end
			randomCode[i] = code
		end
		if randomType == def.TK_FLOAT then
			return true, curMemberIndex, functionNames.randomFloat .. "(" .. randomCode[1] .. ", " .. randomCode[2] .. ")"
		elseif randomType == def.TK_INT then
			return true, curMemberIndex, functionNames.randomInt .. "(" .. randomCode[1] .. ", " .. randomCode[2] .. ")"
		else
			error("Random type neither float nor int", 2)
		end
	elseif memberType == def.ID_TAG then
		-- enough members?
		if curMemberIndex + 1 > #members then
			return false
		end
		-- read members
		local success, nameCode
		success, curMemberIndex, nameCode = ValidateParameter(members, curMemberIndex,
			{
				[def.TK_STRING] = true,
				[def.ID_GET] = {[def.TK_STRING] = true,},
			})
		if not success then
			return false
		end
		local tagTypeMember = members[curMemberIndex]
		curMemberIndex = curMemberIndex + 1
		-- right type?
		if tagTypeMember.type ~= def.TK_FLOAT then
			return false
		end
		if tagTypeMember.data == def.TYPE_ANGLES then
			return true, curMemberIndex, functionNames.getEntity .. "(" .. nameCode .. "):" .. functionNames.getAngles .. "()"
		elseif tagTypeMember.data == def.TYPE_ORIGIN then
			return true, curMemberIndex, functionNames.getEntity .. "(" .. nameCode .. "):" .. functionNames.getOrigin .. "()"
		else
			return false
		end
	elseif memberType == def.TK_GREATER_THAN then
		return true, curMemberIndex, " > "
	elseif memberType == def.TK_LESS_THAN then
		return true, curMemberIndex, " < "
	elseif memberType == def.TK_EQUALS then
		return true, curMemberIndex, " == "
	elseif memberType == def.TK_NOT then
		return true, curMemberIndex, " ~= "
	else
		-- unhandled type? then I entered something invalid in allowed, that's an error in the script.
		error("invalid allowed parameter type", 2)
	end
	error("Should not be reached, I forgot a return somewhere")
end

-- returns true, curMemberIndex, value (as a number) or false, errorMessage
local function GetFloatParameter(members, curMemberIndex)
	local member = members[curMemberIndex]
	if not member then return false, "parameter not there" end
	if member.type ~= def.TK_FLOAT then return false, "non-float" end
	return true, curMemberIndex+1, member.data
end

-- how commands start in Lua - either a string or a function returning a string or false, errorMessage

local commandStarts =
{
	[def.ID_IF] = "if ",
	[def.ID_ELSE] = "else",
	[def.ID_AFFECT] = functionNames.affect,
	[def.ID_LOOP] = "", -- handled completely in commandParameterHandlers since it could either be while or for
	[def.ID_TASK] = functionNames.task,
	[def.ID_PRINT] = functionNames.print,
	[def.ID_RUN] = functionNames.run,
}
-- how command parameters (members) are to be handled - functions returning a string or false, errorMessage
-- only required if #members ~= 0

local commandParameterHandlers =
{
	-- loop(iterations)
	[def.ID_LOOP] = function(self)
		local success, errorMessage, iterations = GetFloatParameter(self.members, 1)
		if not success then
			return false, "Loop amount: " .. errorMessage
		end
		-- endless loop?
		if iterations == -1 then
			return "while true do"
		else
			return "for i = 1, " .. string.format("%.0f", iterations) .. " do"
		end
	end,

	-- if(leftOperand, operator, rightOperand)
	[def.ID_IF] = function(self)
		local curMember = 1
		-- first operand
		local success, curMember, leftOperand = ValidateParameter(self.members, curMember,
			{
				[def.TK_VECTOR] = functionNames.vector,
				[def.TK_FLOAT] = true,
				[def.TK_STRING] = true,
				[def.TK_CHAR] = true,
				[def.ID_GET] =
				{
					[def.TK_FLOAT] = true,
					[def.TK_STRING] = true,
					[def.TK_VECTOR] = true,
				},
				[def.ID_RANDOM] = def.TK_FLOAT,
				[def.ID_TAG] = true,
			})
		if not success then return false, "invalid first if operand" end

		-- operator
		local success, curMember, operator = ValidateParameter(self.members, curMember,
			{
				[def.TK_EQUALS] = true,
				[def.TK_GREATER_THAN] = true,
				[def.TK_LESS_THAN] = true,
				[def.TK_NOT] = true,
			})
		if not success then return false, "invalid if operator" end

		-- second operand
		local success, curMember, rightOperand = ValidateParameter(self.members, curMember,
			{
				[def.TK_VECTOR] = functionNames.vector,
				[def.TK_FLOAT] = true,
				[def.TK_STRING] = true,
				[def.TK_CHAR] = true,
				[def.ID_GET] =
				{
					[def.TK_FLOAT] = true,
					[def.TK_STRING] = true,
					[def.TK_VECTOR] = true,
				},
				[def.ID_RANDOM] = def.TK_FLOAT,
				[def.ID_TAG] = true,
			})
		if not success then return false, "invalid first if operand" end

		if curMember <= #self.members then
			return false, "if with to many parameters"
		end

		-- if one operand is definitely an Angle (ends with "GetAngle()") and the other might be one, too (starts with "vector("), make the other an Angle.
		if leftOperand:sub(- #functionNames.getAngles - 2, -1) == functionNames.getAngles.."()" and rightOperand:sub(1, #functionNames.vector + 1) == functionNames.vector .. "(" then
			rightOperand = functionNames.euler .. rightOperand:sub(#functionNames.vector)
		elseif rightOperand:sub(- #functionNames.getAngles - 2, -1) == functionNames.getAngles.."()" and leftOperand:sub(1, #functionNames.vector + 1) == functionNames.vector .. "(" then
			leftOperand = functionNames.euler .. leftOperand:sub(#functionNames.vector)
		end
		return leftOperand .. operator .. rightOperand .. " then"
	end,

	-- ID_ELSE is trivial, automatically handled (must not have parameters)

	-- Print(string)
	[def.ID_PRINT] = function(self)
		local valid, curMember, memberString = ValidateParameter(self.members, 1,
			{
				[def.TK_STRING] = true,
				[def.TK_FLOAT] = true,
				[def.ID_GET] =
				{
					[def.TK_STRING] = true,
					[def.TK_FLOAT] = true,
				},
				[def.ID_RANDOM] = def.TK_FLOAT,
			})
		if not valid then return false, "Print with invalid parameter" end
		if #self.members >= curMember then return false, "Print with too many parameters" end
		return "(" .. memberString .. ")"
	end,

	-- Affect(target, method)
	[def.ID_AFFECT] = function(self)
		local valid, curMember, target = ValidateParameter(self.members, 1, {[def.TK_STRING] = true, [def.ID_GET] = {[def.TK_STRING] = true}})
		if not valid then return false, "Affect with non-string target" end
		local success, errorMessage, method = GetFloatParameter(self.members, curMember)
		if not success then
			return false, "Affect method: " .. errorMessage
		end
		if method == def.TYPE_FLUSH then
			return "(" .. target .. ", " .. constantNames.flush .. ", function()"
		elseif method == def.TYPE_INSERT then
			return "(" .. target .. ", " .. constantNames.insert .. ", function()"
		else
			return false, "Affect with invalid method parameter (neither flush nor insert)"
		end
	end,

	-- task(name)
	[def.ID_TASK] = function(self)
		local valid, curMember, name = ValidateParameter(self.members, 1, {[def.TK_STRING] = true, [def.ID_GET] = {[def.TK_STRING]= true}})
		if not valid then
			return false, "task with non-string parameter"
		end
		return "(" .. name .. ", function()"
	end,

	-- run(filename)
	[def.ID_RUN] = function(self)
		local valid, curMember, filename = ValidateParameter(self.members, 1, {[def.TK_STRING] = true, [def.ID_GET] = {[def.TK_STRING]= true}})
		if not valid then return false, "run with non-string parameter" end
		return ("(" .. filename .. ")")
	end,
}

-- how commands end in Lua - either a string or a function returning a string or false, errorMessage

local commandEnds =
{
	[icarusInfo.defines.ID_IF] = "end", -- special case "followed by else" handled in Block:ToLua()
	[icarusInfo.defines.ID_ELSE] = "end",
	[icarusInfo.defines.ID_LOOP] = "end",
	[icarusInfo.defines.ID_TASK] = "end)",
	[icarusInfo.defines.ID_AFFECT] = "end)",
}

-- A member is a parameter of an instruction
local Member = {}

local function ReadMember(file)
	local member = Member:New()
	local success, errorMessage = member:ReadFromFile(file)
	if success then
		return member
	else
		return false, errorMessage
	end
end

function Member:New()
	local obj = {}
	setmetatable(obj, self)
	self.__index = self
	return obj
end

-- tries to read the member from the given file
-- returns success, errorMessage
function Member:ReadFromFile(file)
	local success
	success, self.type = file:ReadInt()
	if not success then return false, "could not read member type @"..file:Tell() end
	success, self.len = file:ReadInt()
	if not success then return false, "could not read member length @"..file:Tell() end
	-- char?
	if self.type == icarusInfo.defines.TK_CHAR then
		if self.len == 1 then
			success, self.data = file:ReadChar()
			if not success then return false, "could not TK_CHAR member data @"..file:Tell() end
			return true
		else
			return false, "TK_CHAR member does not have length 1"
		end
	end
	-- int?
	if self.type == icarusInfo.defines.TK_INT then
		if self.len == 4 then
			success, self.data = file:ReadInt()
			if not success then return false, "could not TK_INT member data @"..file:Tell() end
			return true
		else
			return false, "TK_INT member does not have length 4 @"..file:Tell()
		end
	end
	-- float?
	if self.type == icarusInfo.defines.TK_FLOAT and self.len == 4 then
		if self.len == 4 then
			success, self.data = file:ReadFloat()
			if not success then return false, "could not TK_FLOAT member data @"..file:Tell() end
			return true
		else
			return false, "TK_FLOAT member does not have length 4"
		end
	end
	-- string?
	if self.type == icarusInfo.defines.TK_STRING then
		success, self.data = file:ReadString(self.len)
		if not success then return false, "could not TK_STRING member data @"..file:Tell() end
		-- strings are null-terminated, usually.
		local firstnull = self.data:find("\0", 1, true)
		if firstnull then
			self.data = self.data:sub(1, firstnull-1)
		end
		return true
	end
	-- "simple" member type who's data is to be ignored?
	if simpleMembers[self.type] then
		if not file:Seek(file:Tell() + self.len) then return false, "could not skip \"simple\" member @"..file:Tell() end
		return true
	end
	-- member type with no data? (I don't think these exist, even those who don't need any have data)
	if self.len == 0 then
		return true
	end
	-- none of the above
	return false, "unhandled nontrivial member type " .. self.type .. " (" .. (icarusInfo.IDLookup[self.type] or "unknown") .. ") @"..file:Tell()
end

-- A block is either a single instruction (like print()) or a compound statement like an if-(else-)block
local Block = {}

-- creates a new block and reads it from the file
-- returns block or false, errorMessage
local function ReadBlock(file, level)
	local block = Block:New()
	local success, errorMessage = block:ReadFromFile(file, level)
	if not success then
		return false, errorMessage
	end
	return block
end

function Block:New()
	local obj = {}
	setmetatable(obj, self)
	self.__index = self
	return obj
end

-- reads a block from the file - if it's a compound statement block, also reads the children
-- level is the indentation level, i.e. 0 for top level statements and -1 for the Root Block
function Block:ReadFromFile(file, level)
	self.level = level
	if file:EOF() then
		return false, "End of File"
	end
	-- root block contains all other blocks, but no information itself
	if self.level == -1 then
		local success, errorMessage = self:ReadChildren(file)
		-- this only "succeeds" if there are too many ID_BLOCK_ENDs
		if success then
			return false, "Unmatched ID_BLOCK_END @"..file:Tell()
		end
		-- if it "failed" due to EOF, everything's fine.
		if file:EOF() then
			return true
		else
			-- otherwise there was a "real" problem.
			return false, errorMessage
		end
	-- not the root block, but an ordinary block.
	else
		local success
		-- read type
		success, self.type = file:ReadInt()
		if not success then return false, "Could not read Block Type @"..file:Tell() end
		-- read member count
		success, self.numMembers = file:ReadInt()
		if not success then return false, "Could not read Block Length @"..file:Tell() end
		-- read flags
		success, self.flags = file:ReadChar() --chars are read as numbers - okay?
		if not success then return false, "could not read member flags @"..file:Tell() end
		-- can't handle flags, so warn if there are any
		if self.flags ~= 0 then
			jar.Logger.GetDefaultLogger():Warning("IBI Instruction Member at offset " .. file:Tell()-(2*4+1) .. " has flags (unhandled!): "..self.flags)
		end
		self.members = {}
		for i=1, self.numMembers do
			local member, errorMessage = ReadMember(file)
			if not member then
				return false, errorMessage
			end
			self.members[#self.members+1] = member
		end
		if compoundStatements[self.type] then
			local success, errorMessage = self:ReadChildren(file)
			if not success then
				return false, errorMessage
			end
		end
		return true
	end
end

function Block:ReadChildren(file)
	self.children = {}
	-- read until the end of the block is reached (or an error happens)
	while true do
		local block, errorMessage = ReadBlock(file, self.level + 1)
		if block then
			-- block reading successful. It's either...
			if block.type == icarusInfo.defines.ID_BLOCK_END then
				-- ...the last block...
				return true
			else
				-- ...or not the last block.
				self.children[#self.children+1] = block
			end
		else
			-- block reading failed. stop, return error
			return false, errorMessage
		end
	end
	error("Internal Logic Error - infinite loop ended")
end

-- returns true, string or false, errorMessage
function Block:GetLuaCommandStart()
	-- instructions always have to start somehow
	local value = commandStarts[self.type]
	local valueType = type(value)
	if valueType == "string" then
		return true, value
	elseif valueType == "function" then
		local value, errorMessage = value(self)
		if not value then
			return false, errorMessage
		else
			return true, value
		end
	else
		return false, "Instruction type " .. self.type .. " (" .. (icarusInfo.IDLookup[self.type] or "unknown") .. ") not supported"
	end
end

-- returns true, parameterString or false, errorString
function Block:GetLuaCommandParameters()
	local handler = commandParameterHandlers[self.type]
	-- no handler for this command?
	if not handler then
		-- if there are no parameters to be handled, all is fine.
		if #self.members == 0 then
			return true, ""
		else
			return false, "No Parameter Handler for Instructions of type " .. self.type .. " (" .. (icarusInfo.IDLookup[self.type] or "unknown") .. ")"
		end
	end
	local value, errorMessage = handler(self)
	if not value then
		return false, errorMessage
	else
		return true, value
	end
end

function Block:GetLuaCommandEnd()
	local value = commandEnds[self.type]
	local valueType = type(value)
	if type(value) == "function" then
		local value, errorMessage = value(self)
		if not value then
			return false, errorMessage
		else
			return true, value
		end
	else
		return true, value
	end
end

function Block:ToLua(parts, nextBlock)
	parts = parts or {}

	-- Root Block should be skipped since it's not properly handed by Block:GetLuaCommandStart()
	local prefix = ""
	if self.level > -1 then
		prefix = string.rep("\t", self.level)

		-- save command start
		local success, commandStart = self:GetLuaCommandStart()
		if not success then
			return false, commandStart
		end

		-- save parameters

		local success, commandParameters = self:GetLuaCommandParameters()
		if success then
			parts[#parts+1] = prefix .. commandStart .. commandParameters
		else
			return false, commandParameters
		end
	end

	-- save children, if any
	if self.children then
		for index, child in ipairs(self.children) do
			local retval, errorMessage = child:ToLua(parts, self.children[index+1])
			if not retval then
				return false, errorMessage
			end
			-- can disregard retval otherwise, it's parts
		end
	end

	-- save command end, keeping special case "if followed by else" in mind
	if self.type == icarusInfo.defines.ID_IF and nextBlock and nextBlock.type == icarusInfo.defines.ID_ELSE then
		-- special case: "if" doesn't get an end if followed by "else"
	else
		local success, value = self:GetLuaCommandEnd()
		if success and value then
			parts[#parts+1] = prefix .. value
		elseif not success then
			return false, value
		end
	end

	return parts
end

-- input: ibi file (as string)
-- returns: lua file as string or false, errorMessage
function IbiToLua(filename)
	-- open the file
	local file = jar.fs.OpenRead(filename)
	if not file then
		return false, "Could not open " .. ibiname .. ": "..jar.fs.GetLastError()
	end
	-- verify it's an ibi file of the correct version
	local success, ident = file:ReadString(string.len(icarusInfo.ident))
	if not success then return false, "Could not read file ident" end
	if ident ~= icarusInfo.ident then
		file:Close()
		return false, "wrong ident: " .. ident
	end

	local success, version = file:ReadString(string.len(icarusInfo.version))
	if not success then return false, "Could not read file version" end
	if version ~= icarusInfo.version then
		file:Close()
		return false, "wrong version"
	end
	local rootBlock, errorMessage = ReadBlock(file, -1)
	if not rootBlock then
		return false, errorMessage
	end
	local parts, errorMessage = rootBlock:ToLua()
	if not parts then
		return false, errorMessage
	else
		return table.concat(parts, "\n")
	end
end