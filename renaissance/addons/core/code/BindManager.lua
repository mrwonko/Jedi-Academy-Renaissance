require("InstructionBuffer.lua")
require("Instruction.lua")

local KeyToStringMap =
{
	[jar.Key.A] = "a",
	[jar.Key.B] = "b",
	[jar.Key.C] = "c",
	[jar.Key.D] = "d",
	[jar.Key.E] = "e",
	[jar.Key.F] = "f",
	[jar.Key.G] = "g",
	[jar.Key.H] = "h",
	[jar.Key.I] = "i",
	[jar.Key.J] = "j",
	[jar.Key.K] = "k",
	[jar.Key.L] = "l",
	[jar.Key.M] = "m",
	[jar.Key.N] = "n",
	[jar.Key.O] = "o",
	[jar.Key.P] = "p",
	[jar.Key.Q] = "q",
	[jar.Key.R] = "r",
	[jar.Key.S] = "s",
	[jar.Key.T] = "t",
	[jar.Key.U] = "u",
	[jar.Key.V] = "v",
	[jar.Key.W] = "w",
	[jar.Key.X] = "x",
	[jar.Key.Y] = "y",
	[jar.Key.Z] = "z",
	[jar.Key.Num0] = "0",
	[jar.Key.Num1] = "1",
	[jar.Key.Num2] = "2",
	[jar.Key.Num3] = "3",
	[jar.Key.Num4] = "4",
	[jar.Key.Num5] = "5",
	[jar.Key.Num6] = "6",
	[jar.Key.Num7] = "7",
	[jar.Key.Num8] = "8",
	[jar.Key.Num9] = "9",
	[jar.Key.Escape] = "escape",
	[jar.Key.LControl] = "lcontrol",
	[jar.Key.LShift] = "lshift",
	[jar.Key.LAlt] = "lalt",
	[jar.Key.LSystem] = "lsystem",
	[jar.Key.RControl] = "rcontrol",
	[jar.Key.RShift] = "rshift",
	[jar.Key.RAlt] = "ralt",
	[jar.Key.RSystem] = "rsystem",
	[jar.Key.Menu] = "menu",
	[jar.Key.LBracket] = "lbracket",
	[jar.Key.RBracket] = "rbracket",
	[jar.Key.SemiColon] = "semiColon",
	[jar.Key.Comma] = "comma",
	[jar.Key.Period] = "period",
	[jar.Key.Quote] = "quote",
	[jar.Key.Slash] = "slash",
	[jar.Key.BackSlash] = "backslash",
	[jar.Key.Tilde] = "tilde",
	[jar.Key.Equal] = "equal",
	[jar.Key.Dash] = "dash",
	[jar.Key.Space] = "space",
	[jar.Key.Return] = "return",
	[jar.Key.Back] = "back",
	[jar.Key.Tab] = "tab",
	[jar.Key.PageUp] = "pageUp",
	[jar.Key.PageDown] = "pageDown",
	[jar.Key.End] = "end",
	[jar.Key.Home] = "home",
	[jar.Key.Insert] = "insert",
	[jar.Key.Delete] = "delete",
	[jar.Key.Add] = "add",
	[jar.Key.Subtract] = "subtract",
	[jar.Key.Multiply] = "multiply",
	[jar.Key.Divide] = "divide",
	[jar.Key.Left] = "left",
	[jar.Key.Right] = "right",
	[jar.Key.Up] = "up",
	[jar.Key.Down] = "down",
	[jar.Key.Numpad0] = "numpad0",
	[jar.Key.Numpad1] = "numpad1",
	[jar.Key.Numpad2] = "numpad2",
	[jar.Key.Numpad3] = "numpad3",
	[jar.Key.Numpad4] = "numpad4",
	[jar.Key.Numpad5] = "numpad5",
	[jar.Key.Numpad6] = "numpad6",
	[jar.Key.Numpad7] = "numpad7",
	[jar.Key.Numpad8] = "numpad8",
	[jar.Key.Numpad9] = "numpad9",
	[jar.Key.F1] = "f1",
	[jar.Key.F2] = "f2",
	[jar.Key.F3] = "f3",
	[jar.Key.F4] = "f4",
	[jar.Key.F5] = "f5",
	[jar.Key.F6] = "f6",
	[jar.Key.F7] = "f7",
	[jar.Key.F8] = "f8",
	[jar.Key.F9] = "f9",
	[jar.Key.F10] = "f10",
	[jar.Key.F11] = "f11",
	[jar.Key.F12] = "f12",
	[jar.Key.F13] = "f13",
	[jar.Key.F14] = "f14",
	[jar.Key.F15] = "f15",
	[jar.Key.Pause] = "pause",
}

local function KeyToString(key)
	return KeyToStringMap[key]
end

local function JoyAxisToString(joyIndex, axis, amount)
	--TODO: joystick/index management
	local sign = "+"
	if amount < 0 then
		sign = "-"
	end
	return "joy"..joyIndex.."_axis"..(axis+1) .. sign
end

local function JoyButtonToString(joyIndex, button)
	--TODO: joystick/index management
	return "joy"..joyIndex.."_button"..(button+1)
end

local function MouseButtonToString(button)
	return "mouse"..(button+1)
end

local function MouseAxisToString(axis, amount)
	local sign = "+"
	if amount < 0 then
		sign = "-"
	end
	return "mouse_" .. axis .. sign
end

BindManager =
{
	interpreter = nil,
	--todo: default bind file / loadbinds command
	configfile = "config/binds.lua",
}

function BindManager:New(interpreter)
	local obj =
	{
		interpreter = interpreter,
		-- key/...-name -> instructionstring
		binds = {},
	}
	setmetatable(obj, self)
	self.__index = self
	return obj
end

-- takes an instructrion string, turns any instruction starting with a + into one with a -, removes all others.
local function ToMinus(instructionString)
	local result = ""
	local first = true
	-- I've already got instruction string parsing in instruction buffers, so I'll just use them.
	local buf = InstructionBuffer:New(instructionString, 0, 0)
	for _, inst in ipairs(buf.instructions) do
		if inst.instruction:sub(1, 1) == "+" then
			if first then
				result = inst.instruction .. " 0"
				first = false
			else
				result = result .. ";" .. inst.instruction .. " 0"
			end
		end
	end
	return result
end

local function AddAmount(instructionString, amount)
	local result = ""
	local first = true
	-- I've already got instruction string parsing in instruction buffers, so I'll just use them.
	local buf = InstructionBuffer:New(instructionString, 0, 0)
	for _, inst in ipairs(buf.instructions) do
		if inst.instruction:sub(1, 1) == "+" then
			if first then
				result = inst.instruction .. " " .. amount
				first = false
			else
				result = result .. ";" .. inst.instruction .. " " .. amount
			end
		else
			jar.Logger.GetDefaultLogger:Warning("There's an axis bind without +: \"" .. instructionString .. "\" It shouldn't be there and is ignored.")
		end
	end
	return result
end

function BindManager:OnEvent(event)
	local function ExecuteBind(key)
		if self.binds[key] then
			self.interpreter:Interpret(self.binds[key])
			return true
		end
		return false
	end
	local function ExecuteAxisBind(key, amount)
		if self.binds[key] then
			self.interpreter:Interpret(AddAmount(self.binds[key], math.abs(amount))) --abs since I use axisN+ and axisN- for sign.
			return true
		end
		return false
	end
	if event.Type == jar.Event.KeyPressed then
		return ExecuteBind(KeyToString(event.Key.Code))
	elseif event.Type == jar.Event.KeyReleased then
		--keep in mind: only send minus events for those that had plus events.
		local key = KeyToString(event.Key.Code)
		if self.binds[key] then
			self.interpreter:Interpret(ToMinus(self.binds[key]))
			return true
		end
		return false
	elseif event.Type == jar.Event.JoyButtonPressed then
		return ExecuteBind(JoyButtonToString(event.JoyButton.JoyIndex, event.JoyButton.Button))
	elseif event.Type == jar.Event.JoyButtonReleased then
		local button = JoyButtonToString(event.JoyButton.JoyIndex, event.JoyButton.Button)
		if self.binds[button] then
			self.interpreter:Interpret(ToMinus(self.binds[button]))
			return true
		end
		return false
	elseif event.Type == jar.Event.MouseButtonPressed then
		return ExecuteBind(MouseButtonToString(event.MouseButton.Button))
	elseif event.Type == jar.Event.MouseButtonReleased then
		local button = MouseButtonToString(event.MouseButton.Button)
		if self.binds[button] then
			self.interpreter:Interpret(ToMinus(self.binds[button]))
			return true
		end
		return false
	elseif event.Type == jar.Event.MouseWheelMoved then
		if event.MouseWheel.Delta > 0 then
			return ExecuteBind("mwheelup")
		else
			return ExecuteBind("mwheeldown")
		end
	elseif event.Type == jar.Event.MouseMoved then
		--todo: *sensitivity
		--todo: call once with amount = 0 if no moved event in a whole frame - this is totally bugged atm.
		local ret = false
		--[[ I'd better not use this for now. It doesn't work properly.
		if event.MouseMove.X ~= 0 and ExecuteAxisBind(MouseAxisToString("x", event.MouseMove.X), event.MouseMove.X) then
			ret = true
		end
		if event.MouseMove.Y ~= 0 and ExecuteAxisBind(MouseAxisToString("y", event.MouseMove.Y), event.MouseMove.Y) then
			ret = true
		end
		--]]
		return ret
	elseif event.Type == jar.Event.JoyAxisMoved then
		return ExecuteAxisBind(JoyAxisToString(event.JoyAxis.JoyIndex, event.JoyAxis.Axis, event.JoyAxis.Position), event.JoyAxis.Position)
	end
	return false
end

function BindManager:RegisterBindCommand(ccommandManager)
	return ccommandManager:RegisterCommand
	{
		name = "bind",
		description = "Binds an instruction string (= command & cvar usage) to a button/axis.\
Usage: bind [button/axis] [instruction string]",
		Execute = function(_, key, ...)
			if not key then
				print("Usage: bind [button/axis] [instruction string]")
				return
			end
			if #arg == 0 then
				if self.binds[key] then
					print("\"" .. key .. "\" = \"" .. self.binds[key] .. "\"")
				else
					print("\"" .. key .. "\" is not bound.")
				end
				return
			end
			self:Bind(key, table.concat(arg, " "))
		end
	}
	and ccommandManager:RegisterCommand
	{
		name = "unbind",
		description = "Unbinds a button/axis, i.e. reverts bind.\
Usage: unbind [button/axis]",
		Execute = function(_, key, ...)
			if not key then
				print("Usage: bind [button/axis]")
				return
			end
			if self.binds[key] then
				self.binds[key] = nil
			end
		end
	}
	and ccommandManager:RegisterCommand
	{
		name = "bindlist",
		description = "Lists all binds.",
		Execute = function()
			local sorted = {}
			for key, bind in pairs(self.binds) do
				table.insert(sorted, {key = key, bind = bind})
			end
			table.sort(sorted, function(a, b) return a.key < b.key end)
			local str = ""
			for _, info in ipairs(sorted) do
				str = str .. "\"" .. info.key .. "\" = \"" .. info.bind .. "\"\n"
			end
			str = str:sub(1, -2)
			print(str)
		end
	}
end

--[[!	\brief Binds an action (i.e. instructionstring) to an event (i.e. key/joy button press or joy axis/mouse move)
--]]
function BindManager:Bind(event, action)
	if type(action) ~= "string" or type(event) ~= "string" then
		jar.Logger.GetDefaultLogger():Warning("BindManager:Bind(): Invalid parameter!")
		if DEBUG then jar.Logger.GetDefaultLogger():Warning(debug.traceback()) end
		return false
	end
	event = event:lower()
	local function IsValidKey()
		for _, name in pairs(KeyToStringMap) do
			if event == name then
				return true
			end
		end
		return false
	end
	local function IsValidJoyButton()
		local match, button = event:match("(joy%d+_button(%d+))")
		if match ~= event then
			return false
		end
		button = tonumber(button)
		return button > 0
	end
	local function IsValidJoyAxis()
		local match, axis = event:match("(joy%d+%_axis(%d+)[%+%-])")
		if match ~= event then
			return false
		end
		axis = tonumber(axis)
		return (axis > 0)
	end
	local function IsValidMouseButton()
		local match, button = event:match("(mouse(%d))")
		if match ~= event then
			return false
		end
		button = tonumber(button)
		return (button >= 1 and button <= 5)
	end
	local function IsValidMouseAxis()
		return (event == event:match("mouse_[xy][%+%-]"))
	end
	local function IsValidMouseWheel()
		return (event == "mwheelup" or event == "mwheeldown")
	end
	local function IsValidAxisAction()
		return (action:match("%+[^%s]+") == action)
	end
	-- I tried to keep the order from fast to slow
	if IsValidMouseWheel() or IsValidMouseButton() or IsValidJoyButton() or IsValidKey() then
		self.binds[event] = action
		return true
	end
	if IsValidMouseAxis() or IsValidJoyAxis() then
		if IsValidAxisAction() then
			self.binds[event] = action
			return true
		else
			print("Invalid parameter: \"" .. action .."\" - only commands starting with + and no parameters can be bound to axes! (Anything else makes no sense.)")
			return false
		end
	end
	print("Invalid parameter: \""..event.."\"!")
	return false
end

function BindManager:Save()
	local file = jar.fs.OpenWrite(self.configfile)
	if not file then
		jar.Logger.GetDefaultLogger():Error("Could not write file " .. self.configfile .."! " .. jar.fs.GetLastError())
		return false
	end
	file:WriteString("-- automatically generated bind list. Changes to the layout and comments will be discarded on next save.\n")

	local function Escape(str)
		local output = ""
		for i = 1, #str do
			local char = str:sub(i, i)
			--escape any escape signs and quotes
			if char == "\\" or char == "\"" then
				char = "\\" .. char
			--and newlines
			elseif char == "\n" then
				char = "\\n"
			--is this even possible?
			elseif char == "\0" then
				char = "\\0"
			end
			output = output .. char
		end
		return output
	end
	for key, bind in pairs(self.binds) do
		file:WriteString("_G[\"" ..tostring(key) .. "\"] = \"" .. Escape(bind) .. "\"\n")
	end
	file:Close()
	return true
end

function BindManager:Load()
	local code, err = loadfile(self.configfile)
	if not code then
		jar.Logger.GetDefaultLogger():Info("Could not load bind list " .. self.configfile ..": " .. err, 0)
		return false
	end
	-- create secure function environment
	local binds = {}
	binds._G = binds
	setfenv(code, binds)
	-- try and execute file
	local success, err = pcall(code)
	if not success then
		-- caught an error
		jar.Logger.GetDefaultLogger():Warning(self.configfile.." contains an error: " .. err)
		return false
	end
	-- for each setting:
	for name, val in pairs(binds) do
		if name ~= "_G" then
			self:Bind(name, val)
		end
	end
	return true
end