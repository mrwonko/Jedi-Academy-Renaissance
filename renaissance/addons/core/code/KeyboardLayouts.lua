require("CVarManager.lua")
require("CVar.lua")

-- global so mods can add more
KeyboardLayouts = KeyboardLayouts or {}
KeyboardLayouts["us"] =
{
	normal = 
	{
		[jar.Key.A] = 'a',
		[jar.Key.B] = 'b',
		[jar.Key.C] = 'c',
		[jar.Key.D] = 'd',
		[jar.Key.E] = 'e',
		[jar.Key.F] = 'f',
		[jar.Key.G] = 'g',
		[jar.Key.H] = 'h',
		[jar.Key.I] = 'i',
		[jar.Key.J] = 'j',
		[jar.Key.K] = 'k',
		[jar.Key.L] = 'l',
		[jar.Key.M] = 'm',
		[jar.Key.N] = 'n',
		[jar.Key.O] = 'o',
		[jar.Key.P] = 'p',
		[jar.Key.Q] = 'q',
		[jar.Key.R] = 'r',
		[jar.Key.S] = 's',
		[jar.Key.T] = 't',
		[jar.Key.U] = 'u',
		[jar.Key.V] = 'v',
		[jar.Key.W] = 'w',
		[jar.Key.X] = 'x',
		[jar.Key.Y] = 'y',
		[jar.Key.Z] = 'z',
		[jar.Key.Num0] = '0',
		[jar.Key.Num1] = '1',
		[jar.Key.Num2] = '2',
		[jar.Key.Num3] = '3',
		[jar.Key.Num4] = '4',
		[jar.Key.Num5] = '5',
		[jar.Key.Num6] = '6',
		[jar.Key.Num7] = '7',
		[jar.Key.Num8] = '8',
		[jar.Key.Num9] = '9',
		[jar.Key.Numpad0] = '0',
		[jar.Key.Numpad1] = '1',
		[jar.Key.Numpad2] = '2',
		[jar.Key.Numpad3] = '3',
		[jar.Key.Numpad4] = '4',
		[jar.Key.Numpad5] = '5',
		[jar.Key.Numpad6] = '6',
		[jar.Key.Numpad7] = '7',
		[jar.Key.Numpad8] = '8',
		[jar.Key.Numpad9] = '9',
		[jar.Key.LBracket] = '[',
		[jar.Key.RBracket] = ']',
		[jar.Key.SemiColon] = ';',
		[jar.Key.Comma] = ',',
		[jar.Key.Period] = '.',
		[jar.Key.Quote] = "'",
		[jar.Key.Slash] = '/',
		[jar.Key.BackSlash] = '\\',
		[jar.Key.Tilde] = '~', --?
		[jar.Key.Equal] = '=',
		[jar.Key.Dash] = '-',
		[jar.Key.Space] = ' ',
		[jar.Key.Return] = '\n',
		[jar.Key.Tab] = '\t',
		[jar.Key.Add] = '+',
		[jar.Key.Subtract] = '-',
		[jar.Key.Multiply] = '*',
		[jar.Key.Divide] = '/',
	},
	shift =
	{
		[jar.Key.A] = 'A',
		[jar.Key.B] = 'B',
		[jar.Key.C] = 'C',
		[jar.Key.D] = 'D',
		[jar.Key.E] = 'E',
		[jar.Key.F] = 'F',
		[jar.Key.G] = 'G',
		[jar.Key.H] = 'H',
		[jar.Key.I] = 'I',
		[jar.Key.J] = 'J',
		[jar.Key.K] = 'K',
		[jar.Key.L] = 'L',
		[jar.Key.M] = 'M',
		[jar.Key.N] = 'N',
		[jar.Key.O] = 'O',
		[jar.Key.P] = 'P',
		[jar.Key.Q] = 'Q',
		[jar.Key.R] = 'R',
		[jar.Key.S] = 'S',
		[jar.Key.T] = 'T',
		[jar.Key.U] = 'U',
		[jar.Key.V] = 'V',
		[jar.Key.W] = 'W',
		[jar.Key.X] = 'X',
		[jar.Key.Y] = 'Y',
		[jar.Key.Z] = 'Z',
		[jar.Key.Num1] = '!',
		[jar.Key.Num2] = '@',
		[jar.Key.Num3] = '#',
		[jar.Key.Num4] = '$',
		[jar.Key.Num5] = '%',
		[jar.Key.Num6] = '^',
		[jar.Key.Num7] = '&',
		[jar.Key.Num8] = '*',
		[jar.Key.Num9] = '(',
		[jar.Key.Num0] = ')',
		[jar.Key.LBracket] = '{',
		[jar.Key.RBracket] = '}',
		[jar.Key.SemiColon] = ':',
		[jar.Key.Comma] = '<',
		[jar.Key.Period] = '>',
		[jar.Key.Quote] = '"',
		[jar.Key.Slash] = '?',
		[jar.Key.BackSlash] = '|',
		[jar.Key.Tilde] = '~', --?
		[jar.Key.Equal] = '+',
		[jar.Key.Dash] = '_',
		[jar.Key.Space] = ' ',
	},
}
KeyboardLayouts["ger"] = 
{
	normal = 
	{
		[jar.Key.A] = 'a',
		[jar.Key.B] = 'b',
		[jar.Key.C] = 'c',
		[jar.Key.D] = 'd',
		[jar.Key.E] = 'e',
		[jar.Key.F] = 'f',
		[jar.Key.G] = 'g',
		[jar.Key.H] = 'h',
		[jar.Key.I] = 'i',
		[jar.Key.J] = 'j',
		[jar.Key.K] = 'k',
		[jar.Key.L] = 'l',
		[jar.Key.M] = 'm',
		[jar.Key.N] = 'n',
		[jar.Key.O] = 'o',
		[jar.Key.P] = 'p',
		[jar.Key.Q] = 'q',
		[jar.Key.R] = 'r',
		[jar.Key.S] = 's',
		[jar.Key.T] = 't',
		[jar.Key.U] = 'u',
		[jar.Key.V] = 'v',
		[jar.Key.W] = 'w',
		[jar.Key.X] = 'x',
		[jar.Key.Y] = 'y',
		[jar.Key.Z] = 'z',
		[jar.Key.Num0] = '0',
		[jar.Key.Num1] = '1',
		[jar.Key.Num2] = '2',
		[jar.Key.Num3] = '3',
		[jar.Key.Num4] = '4',
		[jar.Key.Num5] = '5',
		[jar.Key.Num6] = '6',
		[jar.Key.Num7] = '7',
		[jar.Key.Num8] = '8',
		[jar.Key.Num9] = '9',
		[jar.Key.Numpad0] = '0',
		[jar.Key.Numpad1] = '1',
		[jar.Key.Numpad2] = '2',
		[jar.Key.Numpad3] = '3',
		[jar.Key.Numpad4] = '4',
		[jar.Key.Numpad5] = '5',
		[jar.Key.Numpad6] = '6',
		[jar.Key.Numpad7] = '7',
		[jar.Key.Numpad8] = '8',
		[jar.Key.Numpad9] = '9',
		--different part
		[jar.Key.LBracket] = 'ß',
		[jar.Key.RBracket] = '´',
		[jar.Key.SemiColon] = 'ü',
		[jar.Key.Comma] = ',',
		[jar.Key.Period] = '.',
		[jar.Key.Quote] = "ä",
		[jar.Key.Slash] = '#',
		[jar.Key.BackSlash] = '^',
		[jar.Key.Tilde] = 'ö',
		[jar.Key.Equal] = '+',
		--end of differences
		[jar.Key.Dash] = '-',
		[jar.Key.Space] = ' ',
		[jar.Key.Return] = '\n',
		[jar.Key.Tab] = '\t',
		[jar.Key.Add] = '+',
		[jar.Key.Subtract] = '-',
		[jar.Key.Multiply] = '*',
		[jar.Key.Divide] = '/',
	},
	shift =
	{
		[jar.Key.A] = 'A',
		[jar.Key.B] = 'B',
		[jar.Key.C] = 'C',
		[jar.Key.D] = 'D',
		[jar.Key.E] = 'E',
		[jar.Key.F] = 'F',
		[jar.Key.G] = 'G',
		[jar.Key.H] = 'H',
		[jar.Key.I] = 'I',
		[jar.Key.J] = 'J',
		[jar.Key.K] = 'K',
		[jar.Key.L] = 'L',
		[jar.Key.M] = 'M',
		[jar.Key.N] = 'N',
		[jar.Key.O] = 'O',
		[jar.Key.P] = 'P',
		[jar.Key.Q] = 'Q',
		[jar.Key.R] = 'R',
		[jar.Key.S] = 'S',
		[jar.Key.T] = 'T',
		[jar.Key.U] = 'U',
		[jar.Key.V] = 'V',
		[jar.Key.W] = 'W',
		[jar.Key.X] = 'X',
		[jar.Key.Y] = 'Y',
		[jar.Key.Z] = 'Z',
		--different part
		[jar.Key.Num1] = '!',
		[jar.Key.Num2] = '"',
		[jar.Key.Num3] = '§',
		[jar.Key.Num4] = '$',
		[jar.Key.Num5] = '%',
		[jar.Key.Num6] = '&',
		[jar.Key.Num7] = '/',
		[jar.Key.Num8] = '(',
		[jar.Key.Num9] = ')',
		[jar.Key.Num0] = '=',
		[jar.Key.LBracket] = '?',
		[jar.Key.RBracket] = '`',
		[jar.Key.SemiColon] = 'Ü',
		[jar.Key.Comma] = ';',
		[jar.Key.Period] = ':',
		[jar.Key.Quote] = "A",
		[jar.Key.Slash] = '\'',
		[jar.Key.BackSlash] = '°',
		[jar.Key.Tilde] = 'Ö',
		[jar.Key.Equal] = '*',
		--end of differences
		[jar.Key.Dash] = '_',
		[jar.Key.Space] = ' ',
	},
}

function ChangeKeyboardLayout(layout)
	if KeyboardLayouts[layout] then
		curLayout = layout
	else
		local str = "No keyboard layout \"" .. layout .."\" available! Available:"
		for name, _ in pairs(KeyboardLayouts) do
			str = str .. " " .. name
		end
		print(str)
	end
end

g_CVarManager:RegisterCVar
{
	name = "in_keyboardLayout",
	type = CVar.TYPES.STRING,
	defaultValue = "us",
	description = "Changes the keyboard layout",
	IsValid = function(value)
		if KeyboardLayouts[value] then
			return true
		else
			local str = "Available keyboard layouts:"
			for name, _ in pairs(KeyboardLayouts) do
				str = str .. " " .. name
			end
			print(str)
			return false
		end
	end
}

function KeycodeToChar(keycode, shift)
	if shift then
		return KeyboardLayouts[g_CVarManager:GetCVarValue("in_keyboardLayout")].shift[keycode]
	else
		return KeyboardLayouts[g_CVarManager:GetCVarValue("in_keyboardLayout")].normal[keycode]
	end
end