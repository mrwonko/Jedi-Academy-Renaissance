require("InstructionInterpreter.lua")
require("CCommandManager.lua")
require("EventListenerStack.lua") --also creates g_EventListenerStack
require("Console.lua")

jar.Logger.GetDefaultLogger():Log("Initializing Test Suite 1")

g_CCommandManager = CCommandManager:New()

if not CCommandManager:LoadCommands() then
	return
end

g_InstructionInterpreter = InstructionInterpreter:New(g_CVarManager, g_CCommandManager)


g_TestWindow = jar.RenderWindow(800, 600, "Test Window", true, false)

g_Console = Console:New(g_TestWindow:GetWidth(), g_TestWindow:GetHeight()/2, g_InstructionInterpreter, "[select font here]")

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