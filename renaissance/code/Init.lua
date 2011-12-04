-- mount config/
jar.fs.Mount("config/", "/config/", false) --append = false -> look here first!

--Initialize CVars (needs to be done first so log_level is set correctly)
jar.Logger.GetDefaultLogger():Info("Initializing CVar system", 3)
require("CVarManager.lua")
-- please don't rename this variable, it's used (or will be used) in a lot of places.
g_CVarManager = CVarManager:New()


jar.Logger.GetDefaultLogger():Info("Loading saved cvars.", 3)
g_CVarManager:LoadCVars() -- no problem if this fails, means there's no config (or it's invalid)

jar.Logger.GetDefaultLogger():Info("Registering log_level, fs_noBase, fs_baseOnlyArchives and fs_baseNoMods", 3)
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


-- ==FileSystem==


-- register cvars - add the OnChange functions after registration because we don't want it to be called during registration (which is the only time it *can* be changed)

assert(g_CVarManager:RegisterCVar
{
	name = "fs_noBase",
	type = CVar.TYPES.BOOLEAN,
	defaultValue = false,
	description = "Whether the base folder should be ignored altogether. (e.g. Standalone game)",
})
g_CVarManager.CVars["fs_nobase"].OnChange = function(oldVal, newVal)
	jar.Logger.GetDefaultLogger():Info("Restart the game for changes to fs_noBase to take effect.", 0)
end

assert(g_CVarManager:RegisterCVar
{
	name = "fs_baseOnlyArchives",
	type = CVar.TYPES.BOOLEAN,
	defaultValue = true,
	description = "Whether only archives in base (i.e. .pk3 files) should be loaded, or also the non-zipped files in base.",
})
g_CVarManager.CVars["fs_baseonlyarchives"].OnChange = function(oldVal, newVal)
	jar.Logger.GetDefaultLogger():Info("Restart the game for changes to fs_baseOnlyArchives to take effect.", 0)
end

assert(g_CVarManager:RegisterCVar
{
	name = "fs_baseNoMods",
	type = CVar.TYPES.BOOLEAN,
	defaultValue = false,
	description = "Whether only the assetsX.pk3 files should be loaded from ../Base/. If true then fs_baseOnlyArchives is ignored since non-archive files in base are mods, too.",
})
g_CVarManager.CVars["fs_nobase"].OnChange = function(oldVal, newVal)
	jar.Logger.GetDefaultLogger():Info("Restart the game for changes to fs_baseNoMods to take effect.", 0)
end

-- TODO CLArguments -> CVars
jar.Logger.GetDefaultLogger():Log("TODO: (Init.lua): parse arguments for Instructions")

if not g_CVarManager:GetCVarValue("fs_noBase") then
	jar.Logger.GetDefaultLogger():Info("Mounting base", 3)

	local noBaseMods = g_CVarManager:GetCVarValue("fs_baseNoMods")
	local onlyArchives = g_CVarManager:GetCVarValue("fs_baseOnlyArchives")
	-- first mount unzipped stuff in base, if enabled
	if not onlyArchives and not noBaseMods then
		if jar.fs.Mount("../Base/", false) then
			jar.Logger.GetDefaultLogger():Info("Mounted Base/", 0)
		else
			jar.Logger.GetDefaultLogger():Error("Error mounting Base/: " .. jar.fs.GetLastError())
		end
	end

	jar.Logger.GetDefaultLogger():Info("Mounting base assets", 3)
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
end

-- ==Mods==

-- init Mod Manager
jar.Logger.GetDefaultLogger():Info("Initializing mod manager", 3)
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
jar.Logger.GetDefaultLogger():Info("== Executing lua files in code/init/ ==", 1)

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
jar.Logger.GetDefaultLogger():Info("== Executing code/main.lua ==", 1)
dofile("code/Main.lua")