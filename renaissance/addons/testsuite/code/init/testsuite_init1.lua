jar.Logger.GetDefaultLogger():Log("Initializing Test Suite 1")

g_testWindow = jar.RenderWindow(800, 600, "Test Window", true, false)

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