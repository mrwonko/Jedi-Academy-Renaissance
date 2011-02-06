require("AutoComplete.lua")

RegisterCommand
{
	name = "help",
	
	description = "Displays the description of a given console command or cvar. Usage: help [cvar/command name]",
	
	manager = theCommandManager,
	
	OnStart = function(self, name)
		if not name then
			print(self.description)
			return
		end
		
		assert(g_CVarManager)
		
		local function Complete(instruction)
			local available = {} -- list of available commands and cvars
			for _, cvar in pairs(g_CVarManager.CVars) do
				table.insert(available, cvar.name)
			end
			for _, ccom in pairs(self.manager.CCommands) do
				table.insert(available, ccom.name)
			end
			
			return AutoComplete(instruction, available)
		end

		local name, avail = Complete(name)
		if #avail == 0 then
			print("No console command or cvar called \"" .. name .. "\" exists.")
			return
		elseif #avail > 1 then
			for _, name in ipairs(avail) do
				print(name)
			end
			return
		end
		
		-- handle wait explicitly since it's no ordinary command, it's hardcoded
		if name == "wait" then
			print("Pauses the execution for a given amount of miliseconds, for example: say \"hello\"; wait 1000; say \"world\"")
			return
		end
		
		if self.manager.CCommands[name:lower()] then
			print(name)
			print(self.manager.CCommands[name:lower()].description or "[no description available]")
		elseif g_CVarManager.CVars[name:lower()] then
			g_CVarManager.CVars[name:lower()]:Print()
		else
			print("^3Woops, this should not happen.")
		end
	end,
}