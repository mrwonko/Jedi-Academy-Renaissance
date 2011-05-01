require("CVarManager.lua")
require("KeyBindings.lua")

local function IsValidKey(name)
	if type(StringToKeyMap[name]) == 'nil' then
		print("Invalid key! Refer to keylist for a list of valid key names!")
		return false
	end
end

g_CVarManager:RegisterCVar{
	name = "key_up",
	type = CVar.TYPES.STRING,
	defaultValue = "up",
	description = "Key to move the player up. (type keylist for a complete list of available keys)",
	IsValid = IsValidKey,
}

g_CVarManager:RegisterCVar{
	name = "key_down",
	type = CVar.TYPES.STRING,
	defaultValue = "down",
	description = "Key to move the player down. (type keylist for a complete list of available keys)",
	IsValid = IsValidKey,
}

g_CVarManager:RegisterCVar{
	name = "key_nextweapon",
	type = CVar.TYPES.STRING,
	defaultValue = "right",
	description = "Key to select the next weapon. (type keylist for a complete list of available keys)",
	IsValid = IsValidKey,
}

g_CVarManager:RegisterCVar{
	name = "key_lastweapon",
	type = CVar.TYPES.STRING,
	defaultValue = "left",
	description = "Key to select the last weapon. (type keylist for a complete list of available keys)",
	IsValid = IsValidKey,
}

g_CVarManager:RegisterCVar{
	name = "key_shoot",
	type = CVar.TYPES.STRING,
	defaultValue = "space",
	description = "Key to shoot. (type keylist for a complete list of available keys)",
	IsValid = IsValidKey,
}

g_CCommandManager:RegisterCommand
{
	name = "keylist",
	description = "Prints the names of all the available keys (for bindings)",
	Execute = function(self)
		print("a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, escape, lcontrol, lshift, lalt, lsystem, rcontrol, rshift, ralt, rsystem, menu, lbracket, rbracket, semicolon, comma, period, quote, slash, backslash, tilde, equal, dash, space, return, back, tab, pageup, pagedown, end, home, insert, delete, add, subtract, multiply, divide, left, right, up, down, numpad0, numpad1, numpad2, numpad3, numpad4, numpad5, numpad6, numpad7, numpad8, numpad9, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, pause\n\nNote: These names apply to the US keyboard layout, if you have a different one, they may be mixed up.")
	end,
}