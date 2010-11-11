g_addonDir = "addons/"

require("STLHelpers.lua")
require("Mod.lua")


ModManager =
{
	mods = {},
	activeMods = {},
	inactiveMods = {},
}

function ModManager:New()
	obj = {}
	setmetatable(obj, self)
	self.__index = self
	return obj
end

function ModManager:Init()
	jar.Logger.GetDefaultLogger():Log("TODO: ModManager:Init() (or do I?)")
	return true
end

function ModManager:AddAvailableMods()
	--mods are in ./addons/, either directories or .pk3 or .zip (some people can't/won't rename)
	
	--get files in ./addons/
	local filenames = jar.GetFilesInDirectory(g_addonDir)
	for filename in StringVectorIterator(filenames) do
		assert(filename ~= nil)
		--only zip/pk3 files are interesting
		if string.sub(filename, -4) == ".zip" or string.sub(filename, -4) == ".pk3" then
			--get their info & insert them
			local mod, err = Mod:New(filename)
			if not mod then
				jar.Logger.GetDefaultLogger():Warning(err)
			else
				table.insert(self.mods, mod)
			end
		end
	end
	
	--get directories in ./addons/
	local filenames = jar.GetDirectoriesInDirectory(g_addonDir)
	for filename in StringVectorIterator(filenames) do
		--get their info & insert them
		local mod, err = Mod:New(filename .. "/")
		if not mod then
			jar.Logger.GetDefaultLogger():Warning(err)
		else
			table.insert(self.mods, mod)
		end
	end
end