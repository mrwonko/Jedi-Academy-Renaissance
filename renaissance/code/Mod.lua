require("ModVersionInfo.lua")

--[[! The Mod class contains information about a mod (i.e. pk3 or directory in g_addonDir (/addons/) ), usually taken from its modinfo.lua
--]]

if not g_addonDir then
	error("No addonDir defined!")
end

Mod = {}
Mod.GetDependencies = 0
Mod.GetIncompatibilities = 0


--forward declarations
local GetCodeFromDirectory = 0
local GetCodeFromZip = 0

--[[!	\brief Creates a new Mod for the given pk3 file
		\param filename name of a pk3/zip-file or a directory within addons/ (ending with "/")
		\return returns the Mod object or nil, errorMessage on failure.
--]]
function Mod:New(filename)
	local obj = {}
	setmetatable(obj, self)
	self.__index = self
	
	--filename is a GUID, used to save inactive mods.
	obj.filename = filename
	obj.name = filename
	obj.description = "@MODINFO_NO_MODINFO_LUA"
	obj.version = 1
	obj.dependencies = {}
	obj.incompatibilities = {}
	obj.hasIncompatibilities = false --whether any of the incompatibilities actually occurred
	obj.hasUnsatisfiedDependencies = false --whether any of the dependencies are not satisfied
	obj.essential = false -- mods that cannot be disabled
	
	local logger = jar.Logger.GetDefaultLogger()
	
	-- what name to use when this mod is used like a directory, as in "modname.pk3/modinfo.lua has a bug"
	
	--get content of modinfo.lua
	local code, err
	if filename:sub(-4) == ".pk3" or filename:sub(-4) == ".zip" then
		code, err = GetCodeFromZip(filename)
	elseif filename:sub(-1) == "/" then
		code, err = GetCodeFromDirectory(filename)
	else
		error("ModInfo:New(): filename has invalid extension!", 2)
	end
	
	--if modinfo.lua doesn't exist, 0 is returned
	if code == 0 then
		--in which case we're done here
		return
	--in case of an error -1, something went wrong
	elseif code == -1 then
		error(err, 2)
	end
	
	--there is a modinfo.lua and we have its content. try compiling it.
	
	code, err = loadstring(code, "@" .. filename .. (filename:sub(-1) == "/" and "" or "/") .. "modinfo.lua")
	if not code then --don't continue if modinfo.lua contained errors since it may contain critical information.
		error(filename .." not loaded due to an error in its modinfo.lua: " .. err, 2)
	end
	
	--execute modinfo.lua in a safe environment
	
	--save environment for modinfo.lua - no functions available, doesn't modify global namespace.
	local modinfoFEnv = {}
	setfenv(code, modinfoFEnv)
	
	--for some reason pcall started crashing at some point when the function environment is empty.
	--modinfoFEnv.blamThisPieceOfCrap = "assigning some value in the function environment seems to fix a crash."
	
	--execute modinfo.lua and catch any errors. Since modinfo may be vital, don't load a mod with a broken modinfo.lua
	local status, err = pcall(code)
	if not status then --errors
		--error(filename .." not loaded due to an error in its modinfo.lua: " .. err, 2)
	end
	
	--[[! \brief safety function to prevent code injection, crashes etc.
		
		Returns unsafe if its type is desiredType, otherwise defaultValue. name is for logging purposes.
	--]]
	local function sanitize(name, desiredType, defaultValue)
		if modinfoFEnv[name] == nil then
			logger:Info(filename .. "'s modinfo.lua doesn't contain " .. name .. ", using default value ("..tostring(defaultValue)..").", 3)
			obj[name] = defaultValue
			return
		end
		if type(modinfoFEnv[name]) == desiredType then
			obj[name] = modinfoFEnv[name]
			return
		end
		logger:Warning("In " .. filename .. "'s modinfo.lua: " .. name .. " is no "..desiredType..", using default value!")
		obj[name] = defaultValue
	end
	
	sanitize("name", "string", filename)
	sanitize("description", "string", "@MODINFO_NO_DESC")
	sanitize("version", "number", 1)
	sanitize("displayVersion", "string", "-")
	sanitize("author", "string", nil)
	sanitize("email", "string", nil)
	sanitize("website", "string", nil)
	sanitize("serverside", "boolean", false)
	sanitize("clientside", "boolean", false)
	sanitize("essential", "boolean", false)
	-- dependencies and incompatibilities are pretty essential, thus the mod is not loaded when they are incorrect.
	if not obj:GetVersionInfo(modinfoFEnv, "dependencies") then
		error("Error in the dependencies information of " .. filename .. "'s modinfo.lua.", 2)
	end
	if not obj:GetVersionInfo(modinfoFEnv, "incompatibilities") then
		error("Error in the incompatibilities information of " .. filename .. "'s modinfo.lua.", 2)
	end
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
	
	local function makeVersionString(info)
		if info.isCorrectVersion then return "(custom version function)" end
		if info.version then return "version " .. info.version end
		if info.minVersion and info.maxVersion then return " version " .. info.minVersion .. " - " .. info.maxVersion end
		if info.minVersion then return "at least version " .. info.minVersion end
		if info.maxVersion then return "up to version " .. info.maxVersion end
		return "all versions"
	end
	if self.dependencies then
		log("dependencies:")
		for _, dep in ipairs(self.dependencies) do
			log("	" .. dep.name .. " - ".. makeVersionString(dep))
		end
	end
	if self.incompatibilities then
		log("incompatibilities:")
		for _, inc in ipairs(self.incompatibilities) do
			log("	" .. inc.name .. " - " .. makeVersionString(inc))
		end
	end
	log("")
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
			-- Having no modinfo.lua is ok. Remember closing the file handle.
			file:Close()
			return 0
		else
			-- This means the function failed, remember closing the file handle.
			file:Close()
			return -1, "Error while searching for modinfo.lua in " .. filename .. ": Error " .. status .. "!"
		end
	end
	--getting here means we have a modinfo.lua.
	
	local content
	status, content = file:GetCurrentFileContent()
	if status ~= unz.OK then
		--something went wrong, remember closing the file handle.
		file:Close()
		return -1, "Error while reading " .. filename .. "'s modinfo.lua: Error " .. status .. "!"
	end
	--getting here means file content could be read. zip can be closed now.
	file:Close()
	return content
end

GetCodeFromDirectory = function (dirname)
	local file = jar.fs.OpenRead(g_addonDir .. dirname .. "modinfo.lua")
	if not file then
		--we've got to assume the file doesn't exist since we should make no assumption about the contents of jar.fs.GetLastError()
		return 0
	end
	
	local status, content = file:GetContent()
	--remember to close the handle
	file:Close()
	
	if not status then
		return -1, jar.fs.GetLastError()
	end
	return content
end

-- used to read info for dependencies and incompatibilities
function Mod:GetVersionInfo(modinfo, whatToGet)
	local info = modinfo[whatToGet]
	local logger = jar.Logger.GetDefaultLogger()
	-- is there information?
	if not info then
		-- modders may want to debug their modinfo.lua so I output some debug information if the loglevel is set high enough.
		logger:Info("No " .. whatToGet .. " in " .. self.filename .. "'s modinfo.lua." , 3)
		return true
	end
	-- is it a table?
	if type(info) ~= "table" then
		logger:Warning("Error in " .. self.filename .. "'s modinfo.lua: ".. whatToGet .." is no table!")
		return false
	end
	self[whatToGet] = {}
	local function add(item)
		logger:Info("Adding " .. item.name .. " (version " ..(item:GetDisplayVersion() or "[any]")..") to " .. self.filename .. "'s ".. whatToGet, 3)
		table.insert(self[whatToGet], item)
	end
	-- okay, let's iterate through it.
	for _, curInfo in pairs(info) do
		if type(curInfo) ~= "table" then
			logger:Warning("Error in one of the "..whatToGet.." from " .. self.filename .. "'s modinfo.lua: It's not a table!")
		else
			-- it's a table, continue.
			
			-- the valid, relevant info from the current dependency/incompatibility info
			local item = ModVersionInfo:New()
			
			--get the name first so it can be included in errors
			if type(curInfo.name) ~= "string" then
				logger:Warning("Error in one of the "..whatToGet.." from "..self.filename.."'s modinfo.lua: No name set!")
			else
				item.name = curInfo.name
				
				-- whether no errors occured yet
				local ok = true
				
				-- let's see what's set.
				for key, value in pairs(curInfo) do
				
					if key == "name" then
						--already handled
					
					elseif key == "version" then
						if type(value) ~= "number" then
							logger:Warning("Error in " .. item.name .. " from the "..whatToGet.." of " .. self.filename.."'s modinfo.lua: \"version\" is no number!")
							ok = false
						else
							item.version = value
						end
					
					elseif key == "minVersion" then
						if type(value) ~= "number" then
							logger:Warning("Error in " .. item.name .. " from the "..whatToGet.." of " .. self.filename.."'s modinfo.lua: \"minVersion\" is no number!")
							ok = false
						else
							item.minVersion = value
						end
					
					elseif key == "maxVersion" then
						if type(value) ~= "number" then
							logger:Warning("Error in " .. item.name .. " from the "..whatToGet.." of " .. self.filename.."'s modinfo.lua: \"maxVersion\" is no number!")
							ok = false
						else
							item.maxVersion = value
						end
					
					elseif key == "isCorrectVersion" then
						if type(value) ~= "function" then
							logger:Warning("Error in " .. item.name .. " from the "..whatToGet.." of " .. self.filename.."'s modinfo.lua: \"isCorrectVersion\" is no function!")
							ok = false
						else
							item.isCorrectVersion = value
						end
						
					elseif key == "displayVersion" then
						if type(value) ~= "string" then
							logger:Warning("Error in " .. item.name .. " from the " .. whatToGet .. " of " .. self.filename .. "'s modinfo.lua: \"displayVersion\" is no string!")
							ok = false
						else
							item.displayVersion = value
						end
					
					else --unknown key
						logger:Warning("Warning: Unknown key" .. ( ( type(key) == "string") and " "..key or "" ) .. " in " .. item.name.." from the "..whatToGet.." of "..self.filename.."!")
					end
				end
				
				-- no errors?
				if ok then
					add(item)
				end
			end
		end
	end
	-- none loaded?
	if #self[whatToGet] == 0 then
		self[whatToGet] = nil
	end
	jar.Logger.GetDefaultLogger():Info(self.filename.."'s " .. whatToGet .. " have been read.", 3)
	return true
end

-- whether this mod is incompatible to any of the other mods
function Mod:IsIncompatible(otherMods)
	for _, info in ipairs(self.incompatibilities) do
		for _, otherMod in pairs(otherMods) do
			if otherMod.name == info.name then
				if info:Applies(otherMod) then
					self.hasIncompatibilities = true
					return true
				end
			end
		end
	end
	return false
end

-- whether the available satisfy all dependencies
function Mod:DependenciesSatisfied(availableMods)
	for _, info in ipairs(self.dependencies) do
		local foundit = false
		for _, mod in pairs(availableMods) do
			if mod.name == info.name then
				-- modnames should be unique (won't get loaded otherwise), so I can be sure no other mod will satisfy this either.
				if not info:Applies(mod) then
					self.hasUnsatisfiedDependencies = true
					jar.Logger.GetDefaultLogger():Warning("\"" .. self.name .. "\"'s dependency \"".. info.name .."\" doesn't have the right version ("..(info:GetDisplayVersion() or "[no displayVersion defined]")..").")
					return false
				end
				foundit = true
			end
		end
		if not foundit then
			return false
		end
	end
	return true
end