--mount config/
physFS.Mount("config/", "/config/", false) --append = false -> look here first!

-- Mods - 1.: init Mod Manager
require("ModManager.lua")

g_modManager = ModManager:New()

if not g_modManager:Init() then
	--since we *really* want the ModManager, we abort if it cannot be initialized.
	jar.Logger.GetDefaultLogger():Error("Could not initialize Mod Manager!")
	return
end

--Get mods
g_modManager:AddAvailableMods()

--Activate mods
jar.Logger.GetDefaultLogger():Log("TODO: Activate mods")

--remount config/, so it's after the mods in the path.
physFS.Unmount("config/")
physFS.Mount("config/", "/config/", false) --append = false -> look here first!

--Initialize CVars
jar.Logger.GetDefaultLogger():Log("TODO: initialize CVars")

jar.Logger.GetDefaultLogger():Log("TODO: mount ./, if cvars allow it")

require("Main.lua")