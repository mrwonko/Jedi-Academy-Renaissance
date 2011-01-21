RegisterCommand
{
	name = "help",
	
	description = "Displays the description of a given console command or cvar. Usage: help [cvar/command name]",
	
	manager = theCommandManager,
	
	OnStart = function(self, name)
		if not name then
			print(description)
			return
		end
		
		if self.manager.CCommands[name] then
			print(self.manager.CCommands[name].description or "[no description available]")
		elseif g_CVarManager then
			if g_CVarManager.CVars[name] then
				g_CVarManager.CVars[name]:Print()
			else
				print("No console command or cvar called \"" .. name .. "\" exists.")
			end
		else
			jar.Logger.GetDefaultLogger.Warning("CVar Manager not found, could not check whether a CVar called \"" .. name .. "\" exists!")
		end
	end,
}