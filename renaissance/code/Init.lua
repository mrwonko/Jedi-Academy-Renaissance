--mount config/
physFS.Mount("config/", "/config/", false) --append = false -> look here first!

--TODO: Mod stuff
--require("modmanager.lua")

--Get mods

--Activate mods

--remount config/, so it's after the mods in the path.
physFS.Unmount("config/")
physFS.Mount("config/", "/config/", false) --append = false -> look here first!

--Initialize CVars

--TODO: mount ./, if cvars allow it
require("Main.lua")