print("==== Test Suite Start ====")

g_CVarManager.CVars["log_level"]:Print()

print ""

local str, array = g_CVarManager:AutoComplete("te")
print("/te")
print("/" .. str)
for _, item in ipairs(array) do
	print("  " .. item)
end

local running = true
while running do
	local e = jar.Event
	while jar.EventManager.GetSingleton():GetEvent(e) do
		if e.Type == jar.Event.Closed then
			running = false
		end
	end
	Core.GetSingleton():Update(g_Window:GetFrameTime())
end

--g_CVarManager:SaveCVars()
print("==== Test Suite End ====")