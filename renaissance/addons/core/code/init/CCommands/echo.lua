RegisterCommand
{
	name = "echo",
	
	description = "Prints all parameters on the console (separated by spaces)",
	
	Execute = function(self, ...)
		local arguments = {...}
		local numArguments = #arguments
		if numArguments == 0 then
			return
		end
		printMe = ""
		for i, cur_arg in ipairs(arguments) do
			-- append space if not last argument
			if i < numArguments then
				printMe = printMe .. cur_arg .. " "
			else
				printMe = printMe .. cur_arg
			end
		end
		print(printMe)
	end,
}