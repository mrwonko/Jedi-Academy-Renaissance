if not g_addonDir then
	error("No addonDir defined!")
end
Mod = {}


--forward declarations
local GetCodeFromZip = 0
local GetCodeFromDirectory = 0
local GetDependencies = 0
local GetIncompatibilities = 0

--[[!	\brief Creates a new Mod for the given pk3 file
		\param filename name of a pk3/zip-file or a directory within addons/ (ending with "/")
		\return returns the Mod object or nil, errorMessage on failure.
--]]
function Mod:New(filename)
	local obj =
	{
		--filename is a GUID, used to save inactive mods.
		filename = filename,
		name = filename,
		description = "@MODINFO_NO_MODINFO_LUA",
		version = 1,
	}
	
	setmetatable(obj, self)
	self.__index = self
	
	local filename2 = filename
	
	--get content of modinfo.lua
	local code, err
	print(string.sub(filename, -4))
	if string.sub(filename, -4) == ".pk3" or string.sub(filename, -4) == ".zip" then
		code, err = GetCodeFromZip(filename)
	elseif string.sub(filename, -1) == "/" then
		code, err = GetCodeFromDirectory(filename)
		filename2 = string.sub(filename, 1, -1)
	else
		return nil, "ModInfo.New(): filename has invalid extension!"
	end
	
	--if modinfo.lua doesn't exist, 0 is returned
	if code == 0 then
		--in which case we're done here
		return obj
	--in case of an error -1, error is returned
	elseif code == -1 then
		return nil, err
	end
	
	--there is a modinfo.lua and we have its content. try compiling it.
	
	code, err = loadstring(code)
	if not code then --code haz bugz?
		jar.Logger.GetDefaultLogger():Warning("Ignoring \"modinfo.lua\" from \"" .. filename .."\" due to error in it: " .. err)
		return obj
	end
	
	--execute modinfo.lua in a safe environment
	
	--save environment for modinfo.lua - no functions available, doesn't modify global namespace.
	local modinfo = {}
	setfenv(code, modinfo)
	
	--execute modinfo.lua and catch any errors
	status, err = pcall(code)
	if not status then --errors
		jar.Logger.GetDefaultLogger():Warning("Ignoring \"modinfo.lua\" from \"" .. filename .."\" due to runtime-error in it: " .. err)
		return obj
	end
	
	--[[! \brief safety function to prevent code injection, crashes etc.
		
		Returns unsafe if its type is desiredType, otherwise defaultValue. name is for logging purposes.
	--]]
	local function sanitize(unsafe, desiredType, name, defaultValue)
		if unsafe == nil then return defaultValue end
		if type(unsafe) == desiredType then
			return unsafe
		else
			jar.Logger.GetDefaultLogger():Warning(filename2 .. "/modinfo.lua: " .. name .. " is no "..desiredType.."!")
			return defaultValue
		end
	end
	
	obj.name = sanitize(modinfo.name, "string", "name", filename)
	obj.description = sanitize(modinfo.description, "string", "description", "@MODINFO_NO_DESC")
	obj.version = sanitize(modinfo.version, "number", "version", 1)
	obj.displayVersion = sanitize(modinfo.displayVersion, "string", "displayVersion", "-")
	obj.author = sanitize(modinfo.author, "string", "author", nil)
	obj.email = sanitize(modinfo.email, "string", "email", nil)
	obj.website = sanitize(modinfo.website, "string", "website", nil)
	obj.serverside = sanitize(modinfo.serverside, "boolean", "serverside", false)
	obj.clientside = sanitize(modinfo.clientside, "boolean", "clientside", false)
	obj.dependencies = GetDependencies(modinfo)
	obj.incompatibilities = GetIncompatibilities(modinfo)
	
	return obj
end

--[[ \brief Prints mod info using the Logger
--]]
function Mod:DebugPrint()
	local log = function (str) return jar.Logger.GetDefaultLogger():Log(str) end
	log("==== Mod: " .. self.name .. " ====")
	log("filename: " .. self.filename)
	log("version: " .. self.displayVersion .. " (" .. self.version .. ")")
	if self.author or self.email then
		log("by " .. (self.author or "[Anonymous]") .. " (" .. (self.email or "[no mail set]") .. ")")
	end
	if self.website then log("Website: " .. self.website) end
	if self.description then log(self.description) end
end

GetCodeFromZip = function (filename)
	local err
	local file = unz.Open(g_addonDir .. filename)
	if not file then
		return nil, "Could not open " .. filename
	end
	
	local status = file:LocateFile("modinfo.lua", 2)
	if status ~= unz.OK then
		if status == unz.END_OF_LIST_OF_FILES then --not found
			--having no modinfo.lua is ok.
			--remember closing the file handle.
			file:Close()
			return 0
		else
			--this means the function failed.
			local err = "Error searching \"" .. filename .. "\" for modinfo.lua, code " .. status .. "!"
			jar.Logger.GetDefaultLogger():Error(err)
			--remember closing the file handle.
			file:Close()
			return -1, err
		end
	end
	--getting here means we have a modinfo.lua.
	
	local code
	status, code = file:GetCurrentFileContent()
	if status ~= unz.OK then --something went wrong
		local err = "Error reading \"modinfo.lua\" from \"" .. filename .. "\", code " .. status .. "!"
		jar.Logger.GetDefaultLogger():Error(err)
		--remember closing the file handle.
		file:Close()
		return -1, err
	end
	--getting here means file content could be read. zip can be closed now.
	file:Close()
	return code
end

GetCodeFromDirectory = function (dirname)
	local file = physFS.OpenRead(g_addonDir .. dirname .. "modinfo.lua")
	if not file then
		--we've got to assume the file doesn't exist since we should make no assumption about the contents of physFS.GetLastError()
		return 0
	end
	
	local status, code = file:GetContent()
	--remember to close the handle
	file:Close()
	
	if not status then
		return -1, physFS.GetLastError()
	end
	return code
end

GetDependencies = function (modinfo)
	jar.Logger.GetDefaultLogger():Log("ToDo: Mod.lua: GetDependencies()")
	return nil
end

GetIncompatibilities = function (modinfo)
	jar.Logger.GetDefaultLogger():Log("ToDo: Mod.lua: GetIncompatibilities()")
	return nil
end