require("InstructionInterpreter.lua")
require("CCommandManager.lua")

jar.Logger.GetDefaultLogger():Log("Initializing Test Suite 1")

g_CCommandManager = CCommandManager:New()

if not CCommandManager:LoadCommands() then
	return
end

g_InstructionInterpreter = InstructionInterpreter:New(g_CVarManager, g_CCommandManager)

-- g_Console = Console:New(g_InstructionInterpreter)

g_testWindow = jar.RenderWindow(800, 600, "Test Window", true, false)

class "LuaLogger" (jar.Logger)

function LuaLogger:__init()
	jar.Logger.__init(self)
end

function LuaLogger:Log(message)
	if not message then
		if DEBUG then
			print("LuaLogger:Warning() called without a message!" .. "\n"..debug.traceback() .. "\n")
		end
		return
	end
	print("Log: " .. message)
end

function LuaLogger:Warning(message)
	if not message then
		if DEBUG then
			print("LuaLogger:Warning() called without a message!" .. "\n"..debug.traceback() .. "\n")
		end
		return
	end
	print("Warning: " .. message)
end

function LuaLogger:Error(message)
	if not message then
		if DEBUG then
			print("LuaLogger:Warning() called without a message!" .. "\n"..debug.traceback() .. "\n")
		end
		return
	end
	print("Error: " .. message)
end

--[[
g_originalDefaultLogger = jar.Logger.GetDefaultLogger()
g_testLogger = LuaLogger()
jar.Logger.SetDefaultLogger(g_testLogger)
jar.Logger.GetDefaultLogger():Log("Test log")
--]]

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