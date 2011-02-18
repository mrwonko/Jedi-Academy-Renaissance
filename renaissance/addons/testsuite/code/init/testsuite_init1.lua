require("InstructionInterpreter.lua")
require("CCommandManager.lua")
require("EventListenerStack.lua") --also creates g_EventListenerStack
require("Console.lua")
require("FontManager.lua")
require("ImageManager.lua")
require("ConsoleLogger.lua")
require("BindManager.lua")

jar.Logger.GetDefaultLogger():Log("Initializing Test Suite 1")

g_CCommandManager = CCommandManager:New()

if not g_CCommandManager:LoadCommands() then
	return
end

g_InstructionInterpreter = InstructionInterpreter:New(g_CVarManager, g_CCommandManager)

g_TestWindow = jar.RenderWindow(800, 600, "Test Window", true, false)
g_TestWindow:ShowMouseCursor(false)

g_FontManager = FontManager:New()
g_ImageManager = ImageManager:New()

g_Console = Console:New(g_TestWindow:GetWidth(), g_TestWindow:GetHeight()/2, g_InstructionInterpreter, g_FontManager:GetFont("lucida8pt", true))
-- commented out for now so I can do debug output via print in Console:Print
g_Console:BindPrint()

g_ConsoleLogger = ConsoleLogger(g_Console)
jar.Logger.SetDefaultLogger(g_ConsoleLogger)

g_BindManager = BindManager:New(g_InstructionInterpreter, g_TestWindow:GetWidth(), g_TestWindow:GetHeight())
g_BindManager:RegisterBindCommand(g_CCommandManager)
g_BindManager:Load()

g_EventListenerStack:PushListener(g_BindManager)
--register console later so it receives commands earlier - before the bind manager in particular.
g_EventListenerStack:PushListener(g_Console)

g_CCommandManager:RegisterCommand
{
	name = "+test",
	Execute = function(self, value)
		print("Test(" .. value .. ")")
	end
}

--[[
RegisterAction{
	name = "+forward",
	OnBegin = function(value)
		print "moving forward"
	end,
	OnEnd = function()
		print "not moving forward anymore"
	end
}
--]]

print("Testsuite 1 initialized")