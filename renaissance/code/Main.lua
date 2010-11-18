print("Main. Yay.")

print("====")
print("Debug: found mods:")
for _, mod in pairs(g_modManager.mods) do
	mod:DebugPrint()
end
print("====")
print("Debug: active mods:")
for _, mod in pairs(g_modManager.activeMods) do
	print(" - " .. mod.name)
end
print("====")