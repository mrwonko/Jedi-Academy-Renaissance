--[[! Console Command Manager
--]]

require("CCommand.lua")
require("AutoComplete.lua")

CCommandManager = {}

	-- name -> command
CCommandManager.CCommands = {}

function CCommandManager:New()
	local obj =
	{
		CCommands = {}
	}
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
	if self.CCommands[info.name:lower()] then
		jar.Logger.GetDefaultLogger():Warning("CCommandManager:RegisterCCommand(): Cannot register multiple CCommands with the same name (\""..info.name.."\")!")
		return false
	end
	--create command and add it
	local cmd = CCommand:New(info)
	if not cmd then
		-- o.O
		return false
	end
	cmd.manager = self
	self.CCommands[info.name:lower()] = cmd
	--yay, worked! Should have, any way...
	return true
end

--[[! \brief Loads all commands from dir or Code/init/CCommands/
--]]
function CCommandManager:LoadCommands(dir)
	dir = dir or "Code/init/CCommands"
	
	-- in case something of this name exists
	local oldCommandManager = theCommandManager
	local oldRegisterCommand = RegisterCommand
	RegisterCommand = function(info) return self:RegisterCommand(info) end
	theCommandManager = self
	local files = jar.fs.GetFilesInDirectory(dir)
	for filename in files:items() do
		if string.sub(filename, -4) == ".lua" then
			jar.Logger.GetDefaultLogger():Info("CCommandManager:LoadCommands(): loading \"" .. dir .. "/" .. filename .. "\"", 3)
			local success, err = pcall(dofile, dir .. "/" .. filename)
			if not success then
				jar.Logger.GetDefaultLogger():Warning(err)
			end
		end
	end
	theCommandManager = oldCommandManager
	RegisterCommand = oldRegisterCommand
	
	return true
end