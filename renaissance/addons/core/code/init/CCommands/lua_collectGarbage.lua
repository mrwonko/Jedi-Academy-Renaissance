RegisterCommand
{
	name = "lua_collectGarbage",
	
	description = "Forces Lua garbage collection. This is usually done every so often when few space is left, but if you feel like forcing it, use this.",
	
	Execute = function(self)
		collectgarbage()
	end,
}