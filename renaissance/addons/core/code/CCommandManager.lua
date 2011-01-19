--[[! Console Command Manager
--]]

require("CCommand.lua")
require("AutoComplete.lua")

CCommandManager =
{
	-- name -> command
	CCommands = {},
}

function CCommandManager:New()
	local obj = {}
	setmetatable(obj, self)
	self.__index = self
	return obj
end

--[[!	\note see CCommand for info description
--]]
function CCommandManager:RegisterCommand(info)
	--validate info, at least the name
	if type(info.name) ~= "string" then
		jar.Logger.GetDefaultLogger():Warning("CCommandManager:RegisterCCommand(): info.name is no string!")
		return false
	end
	--check for duplication
	if self.CCommands[info.name] then
		jar.Logger.GetDefaultLogger():Warning("CCommandManager:RegisterCCommand(): Cannot register multiple CCommands with the same name (\""..info.name.."\")!")
		return false
	end
	--create command and add it
	local cmd = CCommand:New(info)
	if not cmd then
		-- o.O
		return false
	end
	self.CCommands[info.name] = cmd
	--yay, worked! Should have, any way...
	return true
end

--[[! \brief Loads all commands from dir or Code/CCommand/
--]]
function CCommandManager:LoadCommands(dir)
	dir = dir or "Code/CCommand"
	for filename in jar.fs.GetFilesInDirectory(dir) do
		print("todo: init file " .. dir .. "/" .. filename)
	end
	return true
end