print("IBI to Lua converter by Mr. Wonko")

local ibiname = DEBUG_ibiname or "scripts/reverse_engineering/affect.ibi"
local luaname = ibiname .. ".lua" --TODO: proper extension replacement

do --so I can hide everything up to the test code at the bottom

	local failed = false

	--info about ibi files
	local icarusInfo =
	{
		ident = "IBI\0", -- \x[xx] is in decimal
		version = "\195\245\200\63",
		
		defines =
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
		},
		
		IDLookup = 
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
	}

	local outputInfo =
	{
		vectorStart = "Vector3f(",
		vectorEnd = ")",
		getStart = "self:Get(",
		getEnd = ")",
		IDLookup =
		{
			[icarusInfo.defines.ID_SOUND] = "self:PlaySound",
			[icarusInfo.defines.ID_MOVE] = "self:Move",
			[icarusInfo.defines.ID_ROTATE] = "self:Rotate",
			[icarusInfo.defines.ID_WAIT] = "self:Wait", --either self:WaitForTask() or Wait(), depends on param type
			[icarusInfo.defines.ID_SET] = "self:Set",
			[icarusInfo.defines.ID_PRINT] = "self:Print",
			[icarusInfo.defines.ID_USE] = "self:Use",
			[icarusInfo.defines.ID_FLUSH] = "self:Flush",
			[icarusInfo.defines.ID_RUN] = "dofile",
			[icarusInfo.defines.ID_KILL] = "self:Kill",
			[icarusInfo.defines.ID_REMOVE] = "self:Remove",
			[icarusInfo.defines.ID_CAMERA] = "Camera",
			[icarusInfo.defines.ID_DO] = "self:DoTask",
			[icarusInfo.defines.ID_DOWAIT] = "self:DoWaitTask",
			[icarusInfo.defines.ID_SIGNAL] = "self:Signal",
			[icarusInfo.defines.ID_WAITSIGNAL] = "self:WaitSignal",
		},
		
		ToString =
		{
			[icarusInfo.defines.ID_AFFECT] = function(parameters)
				if #parameters ~= 2 then
					print("Error: invalid number of parameters for affect()")
					return false
				end
				local str = "do\
	local target=GetEntity("..parameters[1]..")\
	if not target then\
	warning(\"Affect: Could not find Entity \\\"\".."..parameters[1].."..\"\\\"!\")\
	else\
	local taskname = target:GetUniqueTaskName()\
	target:AddTask(taskname, function(self)"
				if parameters[2] == "" .. icarusInfo.defines.TYPE_FLUSH then
					str = str .. "\
	self:Flush()"
				end
				return str
			end,
			
			[icarusInfo.defines.ID_LOOP] = function(parameters)
				if #parameters ~= 1 then
					print("Error: invalid number of parameters for Loop()")
					return false
				end
				if parameters[1] == -1 then
					return "while true do"
				end
				return "for i = 1, " .. parameters[1] .. " do"
			end,
			
			[icarusInfo.defines.ID_TASK] = function(parameters)
				if #parameters ~= 1 then
					print("Error: invalid number of parameters for task!")
					return false
				end
				return "self:AddTask(\"" .. parameters[1] .. "\", function(self)"
			end,
			
			[icarusInfo.defines.ID_IF] = function(parameters)
				if #parameters ~= 3 then
					print("Error: got more or less than 3 parameters for if()!")
					return false
				end
				local str = "if " .. parameters[1]
				if parameters[2] == "" .. icarusInfo.defines.TK_GREATER_THAN then
					str = str .. " > "
				elseif parameters[2] == "" .. icarusInfo.defines.TK_LESS_THAN then
					str = str .. " < "
				elseif parameters[2] == "" .. icarusInfo.defines.TK_EQUALS then
					str = str .. " == "
				elseif  parameters[2] == "" .. icarusInfo.defines.TK_NOT then
					str = str .. " ~= "
				else
					print("Error: invalid second parameter on if, expected <, >, = or !")
					return false
				end
				return str .. parameters[3] .. " then"
			end,
			
			[icarusInfo.defines.ID_ELSE] = function(parameters)
				if #parameters ~= 0 then
					print("Error: parameters on else!")
					return false
				end
				return "else"
			end,
			
			[icarusInfo.defines.ID_DECLARE] = function(parameters)
				if #parameters ~= 2 then
					print("Error: invalid parameter number for declare!")
					return false
				end
				if parameters[1] == "" .. icarusInfo.defines.TK_FLOAT then
					return string.sub(parameters[2], 2, -2) .. " = 0"
				elseif parameters[1] == "" .. icarusInfo.defines.TK_STRING then
					return string.sub(parameters[2], 2, -2) .. " = \"\""
				elseif parameters[1] == "" .. icarusInfo.defines.TK_VECTOR then
					return string.sub(parameters[2], 2, -2) .. " = " .. outputInfo.vectorStart .. outputInfo.vectorEnd
				else
					print("Error: invalid variable type for declare!")
					return false
				end
			end,
			
			[icarusInfo.defines.ID_FREE] = function(parameters)
				if #parameters ~= 1 then
					print("Not exactly 1 paramter for free()!")
					return false
				end
				return parameters[1] .. " = nil"
			end,
			
			[icarusInfo.defines.ID_PLAY] = function(parameters)
				if #parameters ~= 2 or parameters[1] ~= "\"PLAY_ROFF\"" then
					print("Error: Invalid parameters for Roff, expected \"PLAY_ROFF\" and roff name!")
					return false
				end
				return "self:PlayRoff(" .. parameters[2] .. ")"
			end,
		},
		
		blockEnds =
		{
			[icarusInfo.defines.ID_AFFECT] = "end)\ntarget:DoTask(taskname)\nend\nend",
			[icarusInfo.defines.ID_LOOP] = "end",
			[icarusInfo.defines.ID_TASK] = "end)",
			[icarusInfo.defines.ID_IF] = function(block)
				-- no "end" for if-else
				if not self.parentBlock then
					print("Error: misplaced if!")
					return nil
				end
				if not block.parentBlock.children[block.index+1] or block.parentBlock.children[block.index+1].type ~= icarusInfo.defines.ID_ELSE then
					return "end"
				end
				return ""
			end,
			[icarusInfo.defines.ID_ELSE] = "end",
		},
		
		blockStarters =
		{
			[icarusInfo.defines.ID_AFFECT] = true,
			[icarusInfo.defines.ID_IF] = true,
			[icarusInfo.defines.ID_LOOP] = true,
			[icarusInfo.defines.ID_TASK] = true,
			[icarusInfo.defines.ID_ELSE] = true,
		},
	}

	do -- so I can hide all the messy code below

		--Blocks are from { to } (or filestart to eof)
		--note: actually, I'm only using one Block. Turned out I don't need more.
		local Block = {}

		function Block:New(type)
			local obj =
			{
				children = {}, --children are either Instructions or Blocks		instructions = {},
				type = type, --may be nil
			}
			setmetatable(obj, self)
			self.__index = self
			return obj
		end

		local masterBlock = Block:New()
		local curBlock = masterBlock

		function Block:ToString()
			local str = ""
			for i, child in ipairs(self.children) do
				local childstr = child:ToString()
				if not childstr then
					return nil
				end
				str = str .. childstr .. "\n"
			end
			if outputInfo.blockEnds[self.type] then
				if type(outputInfo.blockEnds[self.type]) == "string" then
					str = str .. outputInfo.blockEnds[self.type]
				else
					local ret = outputInfo.blockEnds[self.type](self)
					if not ret then
						return false
					end
					str = str .. ret
				end
			end
			return str
		end

		function Block:AddInstruction(inst)
			if inst.type == icarusInfo.defines.ID_BLOCK_END then
				if self.parentBlock == nil then
					print("Error: unexpected block end!")
					failed = true
					return
				end
				curBlock = self.parentBlock
				--print("Block ends:", icarusInfo.IDLookup[self.type])
				return
			end
			table.insert(self.children, inst)
			inst.index = #self.children
		end

		--instructions are things like print, if or }

		local Instruction = {}

		function Instruction:New(parentBlock, type)
			local obj =
			{
				type = type or "",
				parameters = {},
				parentBlock = parentBlock,
				flags = 0,
				canHaveChildBlock = false, --set to true if this can have a child block, like if or affect. Also define SetChildBlock().
				index = -1,
			}
			setmetatable(obj, self)
			self.__index = self
			return obj
		end

		function Instruction:ToString()
			-- some blocks have special ToString functions
			if outputInfo.ToString[self.type] then
				return outputInfo.ToString[self.type](self.parameters)
			end
			
			local str = ""
			if outputInfo.IDLookup[self.type] then
				str = str .. outputInfo.IDLookup[self.type]
			else
				print("Error: Unsupported Block Type:", icarusInfo.defines[self.type] or self.type)
				return nil
			end
			str = str .. "("
			for i, param in ipairs(self.parameters) do
				str = str .. param
				if i ~= #self.parameters then
					str = str .. ", "
				end
			end
			str = str .. ")"
			return str
		end

		-------------------------------------------------

		--[[!	\brief Reads a parameter for a function from the given icarus file at the current position

				A parameter may consist of more than one member, e.g. a vector has 5: <, x, y, z, >
				
				\param file the file from which to read (at current position, changes read position)
				\param membersLeft how many members are left in the file, to check if enough are left e.g. for a vector.
				\return Returns parameter, membersLeft, or nil on error.
		--]]
		local ReadParameter = 0 --forward declaration, since it's partially recursive
		local function ReadParameter(file, membersLeft)
			assert(membersLeft > 0) --TODO: replace with warning + return?
			local param = ""
			--reads a single member
			local function ReadMember()
				assert(membersLeft > 0)
				local member = {}
				member.type, member.size = file:ReadInt(), file:ReadInt()
				if member.type == icarusInfo.defines.TK_FLOAT then
					assert(member.size == 4)
					member.data = file:ReadFloat()
				elseif member.type == icarusInfo.defines.TK_STRING then
					member.data = file:ReadString(member.size)
					--automatically remove null-terminator
					if string.sub(member.data, -1) == "\0" then
						member.data = string.sub(member.data, 1, -2)
					else
						print("Warning: string not null-terminated!")
					end
				else
					member.data = file:ReadString(member.size)
				end
				membersLeft = membersLeft - 1
				return member
			end
			
			local members = {}
			local curMem = ReadMember()
			--simple stuff?
			if curMem.type == icarusInfo.defines.TK_FLOAT then
				return "" .. curMem.data, membersLeft
			elseif curMem.type == icarusInfo.defines.TK_STRING then
				return "\"" .. curMem.data .. "\"", membersLeft
			--vector?
			elseif curMem.type == icarusInfo.defines.TK_VECTOR then
				param = outputInfo.vectorStart
				for i = 3, 1, -1 do
					--vector needs at least 5 members: <, x, y, z, >
					if membersLeft < i then
						print("Error: not enough elements after vector start")
						return nil
					end
					local param2
					param2, membersLeft = ReadParameter(file, membersLeft)
					if not param2 then
						return nil
					end
					param = param .. param2
					if i > 1 then
						param = param .. ", "
					end
				end
				return param .. outputInfo.vectorEnd, membersLeft
			--get?
			elseif curMem.type == icarusInfo.defines.ID_GET then
				if membersLeft < 2 then
					print("Error: not enough elements in get!")
					return nil
				end
				local getType, varName --type of get & name of variable to get
				getType = ReadMember()
				if getType.type ~= icarusInfo.defines.TK_FLOAT then
					print("Error: Invalid get type!")
					return nil
				end
				
				if getType.data == icarusInfo.defines.TK_FLOAT then
					getType = "\"float\""
				elseif getType.data == icarusInfo.defines.TK_STRING then
					getType = "\"string\""
				elseif getType.data == icarusInfo.defines.TK_VECTOR then
					getType = "\"vector\""
				else
					print("Error: Invalid get type!")
					return nil
				end
				
				if membersLeft < 1 then
					print("Error: not enough elements in get!")
					return nil
				end
				varName, membersLeft = ReadParameter(file, membersLeft)
				
				return outputInfo.getStart .. getType .. ", " .. varName .. outputInfo.getEnd, membersLeft
			end
			print("Error: unexpected member:", icarusInfo.IDLookup[curMem.type])
			return nil
		end

		--[[!	\brief Reads a single instruction from an ibi file
				\param file the file handle at right position
				\return the Instruction
		--]]
		local function ReadInstruction(file)
			local inst = Instruction:New(curBlock)
			inst.type, inst.numMembers, inst.flags = file:ReadInt(), file:ReadInt(), file:ReadChar()
			inst.members = {}
			local membersLeft = inst.numMembers
			while membersLeft > 0 do
				local param
				param, membersLeft = ReadParameter(file, membersLeft)
				if not param then
					return nil
				end
				table.insert(inst.parameters, param)
			end
			curBlock:AddInstruction(inst)
			if outputInfo.blockStarters[inst.type] then
				local b = Block:New(inst.type)
				b.parentBlock = curBlock
				curBlock:AddInstruction(b)
				curBlock = b
				--print("New block: " .. icarusInfo.IDLookup[inst.type])
			end
			return curBlock
		end

		-------------------------------------------------
		function ConvertIbiToLua(filename)
			failed = false
			--try to open the file
			local file = jar.fs.OpenRead(ibiname)
			if not file then
				error("Could not open "..ibiname..": "..jar.fs.GetLastError())
			end

			local ident = file:ReadString(string.len(icarusInfo.ident))
			if ident ~= icarusInfo.ident then
				print("No IBI file, invalid ident "..ident.."!")
				file:Close()
				return false
			end

			local version = file:ReadString(string.len(icarusInfo.version))
			if version ~= icarusInfo.version then
				print("Wrong IBI version!")
				file:Close()
				return false
			end

			assert(curBlock == masterBlock)

			while not file:EOF() do
				ReadInstruction(file)
				if curBlock == nil then
					failed = true
					break
				end
			end

			file:Close()

			if curBlock ~= masterBlock and not failed then
				print("Error: Not all blocks have been closed, i.e. missing }! Did you write your own compiler, did you hexedit stuff or did I screw up?")
				failed = true
			end

			if failed then
				print("Failed!")
				return false
			end

			--created Lua script
			local output = "-- Lua script automatically created from "..ibiname.."\n"
			local str = masterBlock:ToString()
			if not str then
				print("Error: Could not create string of converted file!")
				return
			end
			output = output .. str
			return output
		end

	end

end

local output = ConvertIbiToLua(ibiname)

if output then
	print("\n" .. output .. "\n")
	local file = jar.fs.OpenWrite(luaname)
	if not file:WriteString(output) then
		print("Error: Could not write file: " .. jar.fs.GetLastError())
	end
	file:Close()
end