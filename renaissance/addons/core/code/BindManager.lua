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

local function JoyAxisToString(joyIndex, axis)
	return "joy"..joyIndex.."_axis"..axis
end

local function JoyButtonToString(joyIndex, button)
	return "joy"..joyIndex.."_button"..button
end

BindManager =
{
}

function BindManager:New()
	local obj = {}
	setmetatable(obj, self)
	self.__index = self
	return obj
end

-- takes an instructrion string, turns any instruction starting with a + into one with a -, removes all others.
local function ToMinus(instructionString)
	local result
	local first = true
	-- I've already got instruction string parsing in instruction buffers, so I'll just use them.
	local buf = InstructionBuffer:New(instructionString, 0, 0)
	for _, inst in ipairs(buf.instructions) do
		if inst.instruction:sub(1, 1) == "+" then
			if first then
				result = "-" .. inst.instruction:sub(2)
				first = false
			else
				result = result .. ";-" .. inst.instruction:sub(2)
			end
		end
	end
	return result
end

print(ToMinus("+test;wait;-test;+attack"))

function BindManager:OnEvent(event)
	if event.Type == jar.Event.KeyReleased then
		--keep in mind: only send minus events for those that had plus events.
	end
	return false
end