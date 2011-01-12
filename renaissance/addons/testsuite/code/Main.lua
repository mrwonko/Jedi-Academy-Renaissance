print("==== Test Suite Start ====")

g_CVarManager.CVars["log_level"]:Print()

print ""

local str, array = g_CVarManager:AutoComplete("f")
print("/f")
print("/" .. str)
for _, item in ipairs(array) do
	print("  " .. item)
end

local running = true
while running do
	local e = jar.Event
	while true do
		local success, event = jar.EventManager.GetSingleton():GetEvent()
		if not success then
			break
		end
		
		if event.Type == jar.Event.Closed then
			running = false
		end
	end
	jar.Core.GetSingleton():Update(g_testWindow:GetFrameTime())
end

--g_CVarManager:SaveCVars()
print("==== Test Suite End ====")