RegisterCommand
{
	name = "echo",
	
	description = "prints all parameters on the console (separated by spaces)",
	
	OnStart = function(self, ...)
		if #arg == 0 then
			return
		end
		printMe = ""
		for i, cur_arg in ipairs(arg) do
			if i < #arg then
				printMe = printMe .. cur_arg .. " "
			else
				printMe = printMe .. cur_arg
			end
		end
		print(printMe)
	end,
}