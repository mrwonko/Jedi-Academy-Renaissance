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
			self.console:Print("^3LuaLogger:Warning() called without a message!" .. "\n"..debug.traceback() .. "\n")
			self.oldLogger:Warning("^3LuaLogger:Warning() called without a message!" .. "\n"..debug.traceback() .. "\n")
			if DEBUG then
				self.console:Print("^3" .. debug.traceback())
				self.oldLogger:Warning(debug.traceback())
			end
		end
		return
	end
	print("crash imminent?")
	self.console:Print(message)
	self.oldLogger:Log(message)
end

function ConsoleLogger:Warning(message)
	if not message then
		if DEBUG then
			self.console:Print("^3LuaLogger:Warning() called without a message!" .. "\n"..debug.traceback() .. "\n")
			self.oldLogger:Warning("^3LuaLogger:Warning() called without a message!" .. "\n"..debug.traceback() .. "\n")
			if DEBUG then
				self.console:Print("^3" .. debug.traceback())
				self.oldLogger:Warning(debug.traceback())
			end
		end
		return
	end
	self.console:Print("^3Warning: " .. message)
	self.oldLogger:Warning(message)
end

function ConsoleLogger:Error(message)
	if not message then
		if DEBUG then
			self.console:Print("^3LuaLogger:Warning() called without a message!" .. "\n"..debug.traceback() .. "\n")
			self.oldLogger:Warning("^3LuaLogger:Warning() called without a message!" .. "\n"..debug.traceback() .. "\n")
			if DEBUG then
				self.console:Print("^3" .. debug.traceback())
				self.oldLogger:Warning(debug.traceback())
			end
		end
		return
	end
	self.console:Print("^1Error: " .. message)
	self.oldLogger:Error(message)
end--]]