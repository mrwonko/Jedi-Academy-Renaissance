require("InstructionInterpreter.lua")
require("CCommandManager.lua")
require("EventListenerStack.lua") --also creates g_EventListenerStack
require("Console.lua")
require("FontManager.lua")
require("ImageManager.lua")
require("SoundManager.lua")
require("ConsoleLogger.lua")

-- 8 channels for sounds
-- circumvents a bug due to too many coexisting sound channels if lua doesn't collect garbage in time (which it doesn't.)
local sounds = 
{
	jar.Sound(),
	jar.Sound(),
	jar.Sound(),
	jar.Sound(),
	jar.Sound(),
	jar.Sound(),
	jar.Sound(),
}
local cursound = 1

function GetSound()
	cursound = cursound + 1
	if cursound > #sounds then cursound = 1 end
	return sounds[cursound]
end

jar.Logger.GetDefaultLogger():Log("Initializing Shoot'em Up")

g_CCommandManager = CCommandManager:New()

if not g_CCommandManager:LoadCommands() then
	return
end

g_InstructionInterpreter = InstructionInterpreter:New(g_CVarManager, g_CCommandManager)

g_CVarManager:RegisterCVar{
	name = "r_width",
	type = CVar.TYPES.INT,
	defaultValue = 800,
	minValue = 1,
	description = "The width of the window, in pixels.",
	OnChange = function(oldVal, newVal)
		if g_Window then print("Restart the game for the new resolution to take effect.") end
	end,
}
g_CVarManager:RegisterCVar{
	name = "r_height",
	type = CVar.TYPES.INT,
	defaultValue = 600,
	minValue = 1,
	description = "The height of the window, in pixels.",
	OnChange = function(oldVal, newVal)
		if g_Window then print("Restart the game for the new resolution to take effect.") end
	end,
}
g_CVarManager:RegisterCVar{
	name = "r_borderless",
	type = CVar.TYPES.BOOLEAN,
	defaultValue = false,
	description = "Whether the window should be borderless.",
	OnChange = function(oldVal, newVal)
		if g_Window then print("Restart the game for the change to take effect.") end
	end,
}
g_CVarManager:RegisterCVar{
	name = "r_fullscreen",
	type = CVar.TYPES.BOOLEAN,
	defaultValue = false,
	description = "Whether the game should be run in fullscreen.",
	OnChange = function(oldVal, newVal)
		if g_Window then print("Restart the game for the change to take effect.") end
	end,
}
g_CVarManager:RegisterCVar{
	name = "snd_effectsvolume",
	type = CVar.TYPES.FLOAT,
	defaultValue = 100,
	description = "How loud sound effects should be, from 0 to 1.",
	minValue = 0,
	maxValue = 100,
}
g_CVarManager:RegisterCVar{
	name = "snd_musicvolume",
	type = CVar.TYPES.FLOAT,
	defaultValue = 100,
	description = "How loud sound effects should be, from 0 to 1.",
	min = 0,
	max = 100,
	OnChange = function(oldVal, newVal)
		if g_Music then g_Music:SetVolume(newVal) end
	end,
}

g_Window = jar.RenderWindow(g_CVarManager:GetCVar("r_width"), g_CVarManager:GetCVar("r_height"), "Mr. Wonko's Ludum Dare 20 entry - second try", (not g_CVarManager:GetCVar("r_borderless")), g_CVarManager:GetCVar("r_fullscreen"))
g_Window:EnableKeyRepeat(false) --no automatic key repeat (fast fire would be too easy)
--g_Window:ShowMouseCursor(false)

g_CCommandManager:RegisterCommand
{
	name = "movewindow",
	description = "Moves the window to a given point. Example: movewindow 0 0",
	Execute = function(self, x, y)
		if not x or not y then
			print("Please supply x and y position!")
			return
		end
		g_Window:SetPosition(tonumber(x), tonumber(y))
	end,
}

g_FontManager = FontManager:New()
g_ImageManager = ImageManager:New()
g_SoundManager = SoundManager:New()

g_Console = Console:New(g_Window:GetWidth(), g_Window:GetHeight()/2, g_InstructionInterpreter, g_FontManager:GetFont("lucida8pt", true))
-- commented out for now so I can do debug output via print in Console:Print
g_Console:BindPrint()

g_ConsoleLogger = ConsoleLogger(g_Console)
jar.Logger.SetDefaultLogger(g_ConsoleLogger)

--register console later so it receives commands earlier - before the bind manager in particular.
g_EventListenerStack:PushListener(g_Console)