class "ConsoleLogger" (jar.Logger)
ConsoleLogger.oldLogger = jar.Logger.GetDefaultLogger()
ConsoleLogger.console = g_Console

function ConsoleLogger:__init(console)
	jar.Logger.__init(self)
	self.oldLogger = jar.Logger.GetDefaultLogger()
	self.console = console
end

function ConsoleLogger:Log(message)
	if not message then
		if DEBUG then
			print("^3LuaLogger:Warning() called without a message!" .. "\n"..debug.traceback() .. "\n")
		end
		return
	end
	console:Print(message)
end

function ConsoleLogger:Warning(message)
	if not message then
		if DEBUG then
			print("^3LuaLogger:Warning() called without a message!" .. "\n"..debug.traceback() .. "\n")
		end
		return
	end
	print("^3Warning: " .. message)
	self.oldLogger:Warning(message)
end

function ConsoleLogger:Error(message)
	if not message then
		if DEBUG then
			print("^3LuaLogger:Warning() called without a message!" .. "\n"..debug.traceback() .. "\n")
		end
		return
	end
	print("^1Error: " .. message)
	self.oldLogger:Error(message)
end