--debug: increase log level
jar.Logger.GetDefaultLogger():Log("TODO: delete log level increase in Init.lua")
jar.Logger.GetDefaultLogger():SetLoggingLevel(2)

--mount config/
jar.fs.Mount("config/", "/config/", false) --append = false -> look here first!

-- I may want to initialize some kind of config system here?

--Initialize CVars (needs to be done first so log_level is set correctly)
jar.Logger.GetDefaultLogger():Log("TODO: initialize CVars")

--   Mods
-- init Mod Manager
require("ModManager.lua")

g_modManager = ModManager:New()

if not g_modManager:Init() then
	--since we *really* want the ModManager, we abort if it cannot be initialized.
	jar.Logger.GetDefaultLogger():Error("Could not initialize Mod Manager!")
	return
end

--remount config/, so it's after the mods in the path.
jar.fs.Unmount("config/")
jar.fs.Mount("config/", "/config/", false) --append = false -> look here first!

jar.Logger.GetDefaultLogger():Log("TODO: mount ./, if cvars allow it")

require("Main.lua")