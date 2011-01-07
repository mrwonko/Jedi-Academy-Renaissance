-- mount config/
jar.fs.Mount("config/", "/config/", false) --append = false -> look here first!

--Initialize CVars (needs to be done first so log_level is set correctly)
require("CVarManager.lua")
g_CVarManager = CVarManager:New()

g_CVarManager:LoadCVars() -- no problem if this fails, means there's no config (or it's invalid)

-- register log cvars
g_CVarManager:RegisterCVar{
	name = "log_level",
	type = CVar.TYPES.INT,
	defaultValue = 0,
	description = "The higher the logging level the more (debugging) information will be logged.",
	OnChange = function(oldVal, newVal)
		jar.Logger.GetDefaultLogger():SetLoggingLevel(newVal)
	end,
}

-- TODO CLArguments -> CVars
jar.Logger.GetDefaultLogger():Log("TODO: parse arguments for CVars")


-- ==FileSystem==

g_CVarManager:RegisterCVar{
	name = "fs_baseOnlyArchives",
	type = CVar.TYPES.BOOLEAN,
	defaultValue = true,
	description = "Whether only archives in base (i.e. .pk3 files) should be loaded, or also the non-zipped files in base.",
	OnChange = function(oldVal, newVal)
		jar.Logger.Info("Restart the game for changes to fs_baseOnlyArchives to take effect.", 0)
	end,
}
g_CVarManager:RegisterCVar{
	name = "fs_baseNoMods",
	type = CVar.TYPES.BOOLEAN,
	defaultValue = false,
	description = "Whether only the assetsX.pk3 files should be loaded from ../Base/. If true then fs_baseOnlyArchives is ignored since non-archive files in base are mods, too.",
	OnChange = function(oldVal, newVal)
		jar.Logger.Info("Restart the game for changes to fs_baseNoMods to take effect.", 0)
	end,
}

local noBaseMods = g_CVarManager:GetCVar("fs_baseNoMods")
local onlyArchives = g_CVarManager:GetCVar("fs_baseOnlyArchives")
-- first mount unzipped stuff in base, if enabled
if not onlyArchives and not noBaseMods then
	if jar.fs.Mount("../Base/", false) then
		jar.Logger.GetDefaultLogger():Info("Mounted Base/", 0	)
	else
		jar.Logger.GetDefaultLogger():Error("Error mounting Base/: " .. jar.fs.GetLastError())
	end
end
-- then mount the pk3 archives in base. I don't apply my mod code to them because I'm lazy and because the filenames wouldn't be GUIDs anymore.
for filename in jar.GetFilesInDirectory("../base/") do
	if string.lower(string.sub(filename, -4)) == ".pk3" then
		if not noBaseMods or string.match(string.lower(filename), "assets%d%.pk3") then
			if jar.fs.Mount("../Base/" .. filename, false) then
				jar.Logger.GetDefaultLogger():Info("Mounted Base/" .. filename, 0	)
			else
				jar.Logger.GetDefaultLogger():Error("Error mounting Base/" .. filename .. ": " .. jar.fs.GetLastError())
				return
			end
		end
	end
end

-- ==Mods==

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

-- better safe than sorry: no further filesystem changes!
jar.fs.Mount = nil
jar.fs.Unmount = nil

--execute init code, i.e. all .lua files in code/init/
jar.Logger.GetDefaultLogger():Info("", 1)
jar.Logger.GetDefaultLogger():Info("== Doing initializations ==", 1)

for filename in jar.fs.GetFilesInDirectory("code/init") do
	if string.lower(string.sub(filename, -4)) == ".lua" then
		jar.Logger.GetDefaultLogger():Info("Executing code/init/" .. filename, 2)
		local success, err = pcall(dofile, "code/init/" .. filename)
		if not success then
			jar.Logger.GetDefaultLogger():Warning(err)
		end
	end
end

jar.Logger.GetDefaultLogger():Info("", 1)
jar.Logger.GetDefaultLogger():Info("== Executing main.lua ==", 1)
dofile("code/Main.lua")