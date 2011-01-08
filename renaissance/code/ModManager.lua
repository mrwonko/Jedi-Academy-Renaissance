g_addonDir = "addons/"

require("Mod.lua")


ModManager =
{
	mods = {}, -- all available mods. array since names may be duplicate and using the filename doesn't really help me
	activeMods = {}, -- those mods that have been activated. array so it can be sorted by load order
	inactiveMods = {}, -- the inactive mods. array.
}

function ModManager:New()
	obj = {}
	setmetatable(obj, self)
	self.__index = self
	return obj
end

-- scans /addons/ for mods.
-- basically a private method of ModManager
local function AddAvailableMods(self)
	jar.Logger.GetDefaultLogger():Info("Checking for available mods", 3)
	--mods are in ./addons/, either directories or .pk3 or .zip (some people can't/won't rename)
	
	--get files in ./addons/
	for filename in jar.GetFilesInDirectory(g_addonDir) do
		assert(filename ~= nil and filename ~= "")
		--only zip/pk3 files are interesting
		if string.sub(filename, -4) == ".zip" or string.sub(filename, -4) == ".pk3" then
			--get their info & insert them
			local mod, err = Mod:New(filename)
			if not mod then
				jar.Logger.GetDefaultLogger():Warning(err)
			else
				table.insert(self.mods, mod)
			end
		else
			jar.Logger.GetDefaultLogger():Warning("Warning: addons/" .. filename .. " has an invalid extension (neither .zip nor .pk3 nor folder)!")
		end
	end
	
	--get directories in ./addons/
	for filename in jar.GetDirectoriesInDirectory(g_addonDir) do
		assert(filename ~= nil and filename ~= "")
		--get their info & insert them
		local mod, err = Mod:New(filename .. "/")
		if not mod then
			jar.Logger.GetDefaultLogger():Warning(err)
		else
			table.insert(self.mods, mod)
		end
	end
	return true
end

-- checks which mods have been disabled by the user
-- basically a private method of ModManager
local function LoadAndApplyConfig(self)
	jar.Logger.GetDefaultLogger():Info("loading disabled mod config", 3)
	
	-- get inactive mods
	local file, err = loadfile("config/InactiveMods.lua")
	-- if I can't open that file I assume it doesn't exist (which is ok) although it could contain an error
	if not file then
		jar.Logger.GetDefaultLogger():Info("Could not load config/InactiveMods.lua: "..err, 2)
		return true
	end
	local fileEnv = {}
	setfenv(file, fileEnv)
	local status, err = pcall(file)
	if not status then
		jar.Logger.GetDefaultLogger():Warning("Ignoring config/InactiveMods.lua (i.e. inactive mods settings) due to error: " .. err)
		-- it's not really bad, except that the user's settings are not used.
		return true
	end
	if not fileEnv.inactiveMods then
		jar.Logger.GetDefaultLogger():Warning("config/InactiveMods.lua doesn't contain info about inactive mods!")
		-- well, that's no reason to stop booting...
		return true
	end
	if type(fileEnv.inactiveMods) ~= "table" then
		jar.Logger.GetDefaultLogger():Warning("inactiveMods in config/InactiveMods.lua is no table, thus inactive mods cannot be loaded.")
		-- dito
		return true
	end
	for _, filename in ipairs(fileEnv.inactiveMods) do
		if type(filename) ~= "string" then
			jar.Logger.GetDefaultLogger():Warning("Invalid entry in config/InactiveMods.lua: No string!")
		else
			local found = false
			for _, mod in ipairs(self.mods) do
				if mod.filename == filename then
					table.insert(self.inactiveMods, mod)
					found = true
				end
			end
			if not found then
				jar.Logger.GetDefaultLogger():Info("\"Forgetting\" disabled mod \"" .. filename .. "\" since it no longer exists.", 2)
			end
		end
	end
	
	return true
end

-- activates (i.e. mounts) the non-disabled mods unless they're 
-- basically a private method of ModManager
local function ActivateMods(self)

	-- copy self.mods for the algorithm since we want to keep the original it the way it is
	local mods = {}
	for _, mod in ipairs(self.mods) do
		table.insert(mods, mod)
	end
	
	-- as long as another mod can be added I need to run this again.
	local continue = true
	while continue do
		continue = false
		-- try adding a mod, starting from the beginning
		for index, mod in ipairs(mods) do
			-- may have been put on inactive list by config
			local isInactive = false
			for _, inactive in ipairs(self.inactiveMods) do
				if mod == inactive then
					isInactive = true
					jar.Logger.GetDefaultLogger():Info(mod.filename .. " is disabled.", 3)
					break
				end
			end
			if isInactive then
				table.remove(mods, index)
				-- I don't think iterators still work (correctly) then the table is changed so let's start again
				continue = true
				break
			-- if there is an incompatibility, this mod cannot be added. ever.
			elseif mod:IsIncompatible(self.activeMods) then
				table.remove(mods, index)
				table.insert(self.inactiveMods, mod)
				-- I don't think iterators still work (correctly) then the table is changed so let's start again
				continue = true
				break
			elseif mod:DependenciesSatisfied(self.activeMods) then
				-- this mod should be loaded. not yet though since I still need to order them.
				table.remove(mods, index)
				-- array since it can be sorted
				table.insert(self.activeMods, mod)
				-- we need to run again since mods tested earlier may now have their dependencies satisfied.
				-- I don't think iterators still work (correctly) then the table is changed so let's start again
				continue = true
				break
			end
			--else: this mod can't be added yet, the dependencies aren't satisfied.
		end
	end
	-- anything still left doesn't have its dependencies satisfied. Put them into inactive list and report them.
	for _, mod in ipairs(mods) do
		table.insert(self.inactiveMods, mod)
		jar.Logger.GetDefaultLogger():Warning("The mod \"" .. mod.name .. "\" (" .. mod.filename .. ") has unsatisfied dependencies!")
	end
	
	-- sort the to-be-active mods by filename (for now) - I <3 anonymous functions
	table.sort(self.activeMods, function(first, second)
		--this functions should return true when first is "less" than second.
		--I order the loadorder by filename, just as in base jka.
		--TODO: change loadorder to be adjustable by the user (necessary? can't they just rename?)
		return string.lower(first.filename) < string.lower(second.filename)
		end)
	
	-- mods are now ordered correctly, mount them.
	for _, mod in ipairs(self.activeMods) do
		if not jar.fs.Mount(g_addonDir .. mod.filename, false) then
			-- if it couldn't be mounted then some dependencies may in fact not be satisfied.
			-- note: this should actually not happen since the file was already successfully opened durin modinfo.lua reading.
			jar.Logger.GetDefaultLogger():Error("Error mounting " .. g_addonDir .. mod.filename .. ": " .. jar.fs.GetLastError())
			return false
		else
			jar.Logger.GetDefaultLogger():Info("Mounted "..g_addonDir..mod.filename, 0)
		end
	end
	
	return true
end

function ModManager:Init()
	--Get mods
	if not AddAvailableMods(self) then return false end
	
	--Deactivate those deactivated by the user
	if not LoadAndApplyConfig(self) then return false end

	--Activate mods
	if not ActivateMods(self) then return false end
	
	--save config (since maybe some mods could not be activated)
	self:SaveConfig()
	
	return true
end

-- saves currently inactive mods
function ModManager:SaveConfig()
	local file = jar.fs.OpenWrite("config/InactiveMods.lua")
	if not file then
		return false
	end
	assert(file:WriteString("inactiveMods =\n{\n"))
	for _, mod in ipairs(self.inactiveMods) do
		assert(file:WriteString("	\""..mod.filename.."\",\n"))
	end
	assert(file:WriteString("}"))
	assert(file:Close())
	return true;
end