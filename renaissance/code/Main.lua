print("Main. Yay.")

print("====")
print("Debug: found mods")

for i, mod in ipairs(g_modManager.mods) do
	mod:DebugPrint()
end
print("====")