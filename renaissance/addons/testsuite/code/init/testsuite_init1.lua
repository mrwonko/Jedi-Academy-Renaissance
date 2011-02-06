require("InstructionInterpreter.lua")
require("CCommandManager.lua")
require("EventListenerStack.lua") --also creates g_EventListenerStack
require("Console.lua")
require("FontManager.lua")
require("ConsoleLogger.lua")

jar.Logger.GetDefaultLogger():Log("Initializing Test Suite 1")

g_CCommandManager = CCommandManager:New()

if not g_CCommandManager:LoadCommands() then
	return
end

g_InstructionInterpreter = InstructionInterpreter:New(g_CVarManager, g_CCommandManager)

g_TestWindow = jar.RenderWindow(800, 600, "Test Window", true, false)

g_FontManager = FontManager:New()

g_Console = Console:New(g_TestWindow:GetWidth(), g_TestWindow:GetHeight()/2, g_InstructionInterpreter, g_FontManager:GetFont("lucida8pt", true))
g_Console:BindPrint()

g_ConsoleLogger = ConsoleLogger(g_Console)
jar.Logger.SetDefaultLogger(g_ConsoleLogger)

g_EventListenerStack:PushListener(g_Console)

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