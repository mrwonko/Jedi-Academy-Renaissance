require("InstructionInterpreter.lua")
require("CCommandManager.lua")

jar.Logger.GetDefaultLogger():Log("Initializing Test Suite 1")

g_CCommandManager = CCommandManager:New()

if not CCommandManager:LoadCommands() then
	return
end

g_InstructionInterpreter = InstructionInterpreter:New(g_CVarManager, g_CCommandManager)

g_testWindow = jar.RenderWindow(800, 600, "Test Window", true, false)

class "LuaLogger" (jar.Logger)

function LuaLogger:__init()
	jar.Logger.__init(self)
end

function LuaLogger:Log(message)
	print("Log: " .. message)
end

function LuaLogger:Warning(message)
	print("Warning: " .. message)
end

function LuaLogger:Error(message)
	print("Error: " .. message)
end

print "creating logger object"
g_testLogger = LuaLogger()
print "setting default logger"
jar.Logger.SetDefaultLogger(g_testLogger)
print "doing testlog"
jar.Logger.GetDefaultLogger():Log("Test log")

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