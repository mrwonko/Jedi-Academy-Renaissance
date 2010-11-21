print("Main. Yay.")

g_CVarManager.CVars["log_level"]:Print()

print ""

local str, array = g_CVarManager:AutoComplete("te")
print("/te")
print("/" .. str)
for _, item in ipairs(array) do
	print("  " .. item)
end

--g_CVarManager:SaveCVars()