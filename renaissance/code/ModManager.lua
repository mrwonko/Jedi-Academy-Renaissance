require("STLHelpers.lua")

ModInfo = {}
--[[!	\brief Creates a new ModInfo for the given pk3 file
		\return returns the ModInfo object or nil, errorMessage on failure.
--]]
function ModInfo:New(filename)
	local obj = {}
	--init settings
	--filename is a GUID, used to save inactive mods.
	return nil, "WIP"
end

ModManager =
{
	mods = {},
	activeMods = {},
	inactiveMods = {},
}

function ModManager:Init()
end

function ModManager:AddAvailableMods()
	--get files in ./
		--only zip files are interesting
		--check if they are in the physFS path
		--if not: get their info
end