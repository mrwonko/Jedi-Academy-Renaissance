require("InstructionInterpreter.lua")
require("CCommandManager.lua")
require("EventListenerStack.lua") --also creates g_EventListenerStack
require("Console.lua")
require("FontManager.lua")
require("ImageManager.lua")
require("SoundManager.lua")
require("ConsoleLogger.lua")

jar.Logger.GetDefaultLogger():Log("Initializing LD22 Project")

-- sound stuff

-- 8 channels for sounds, used in order
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

-- gets a sound object which can be used to play a given buffer (jar.Sound)
function GetSound()
	cursound = cursound + 1
	if cursound > #sounds then cursound = 1 end
	return sounds[cursound]
end

-- create a command manager - cvar manager has already been initialized by bootstrapper.
g_CCommandManager = CCommandManager:New()

-- Load the commands from code/init/CCommands/
if not g_CCommandManager:LoadCommands() then
	return
end

-- Create an instruction interpreter - instructions are what you enter in e.g. Quake 3's console
g_InstructionInterpreter = InstructionInterpreter:New(g_CVarManager, g_CCommandManager)

-- Set up some window related variables
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

-- todo: keep in mind to use these sound related variables when playing sounds
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

-- create the window
g_Window = jar.RenderWindow(g_CVarManager:GetCVarValue("r_width"), g_CVarManager:GetCVarValue("r_height"), "Mr. Wonko's LD22 Project (change title in init/ld22_init.lua)", (not g_CVarManager:GetCVarValue("r_borderless")), g_CVarManager:GetCVarValue("r_fullscreen"))

-- todo: do I need to disable key repeat?
g_Window:EnableKeyRepeat(false)
-- todo: should the cursor be hidden?
--g_Window:ShowMouseCursor(false)

-- register a command to place the window at a given position
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

-- create font, image & sound buffer manager
g_FontManager = FontManager:New()
g_ImageManager = ImageManager:New()
g_SoundManager = SoundManager:New()

-- create the console
g_Console = Console:New(g_Window:GetWidth(), g_Window:GetHeight()/2, g_InstructionInterpreter, g_FontManager:GetFont("lucida8pt", true))
-- redirect print() to the console
g_Console:BindPrint()

-- redirect logger to console
g_ConsoleLogger = ConsoleLogger(g_Console)
jar.Logger.SetDefaultLogger(g_ConsoleLogger)

--register console later so it receives commands earlier - before the bind manager in particular.
g_EventListenerStack:PushListener(g_Console)