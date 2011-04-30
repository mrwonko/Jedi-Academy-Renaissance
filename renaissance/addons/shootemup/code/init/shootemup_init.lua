require("InstructionInterpreter.lua")
require("CCommandManager.lua")
require("EventListenerStack.lua") --also creates g_EventListenerStack
require("Console.lua")
require("FontManager.lua")
require("ImageManager.lua")
require("ConsoleLogger.lua")

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
	defaultValue = 800,
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

g_Window = jar.RenderWindow(g_CVarManager:GetCVar("r_width"), g_CVarManager:GetCVar("r_height"), "Mr. Wonko's Ludum Dare 20 entry - second try", (not g_CVarManager:GetCVar("r_borderless")), g_CVarManager:GetCVar("r_fullscreen"))
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

g_Console = Console:New(g_Window:GetWidth(), g_Window:GetHeight()/2, g_InstructionInterpreter, g_FontManager:GetFont("lucida8pt", true))
-- commented out for now so I can do debug output via print in Console:Print
g_Console:BindPrint()

g_ConsoleLogger = ConsoleLogger(g_Console)
jar.Logger.SetDefaultLogger(g_ConsoleLogger)

--register console later so it receives commands earlier - before the bind manager in particular.
g_EventListenerStack:PushListener(g_Console)