RegisterCommand
{
	name = "cvarlist",
	
	description = "Prints the names of all available CVars on the console",
	
	Execute = function(self)
		if not g_CVarManager then
			print("Error, no CVarManager found!")
			return
		end
		local cvars = {}
		for name, _ in pairs(g_CVarManager.CVars) do
			table.insert(cvars, name)
		end
		table.sort(cvars)
		for _, name in ipairs(cvars) do
			print(name)
		end
	end,
}