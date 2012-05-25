require("Joystick.lua")

-- Singleton. Only one exists. Makes access much easier.
JoystickManager =
{
	configfilename = "config/joysticks.lua",
	lastIndex = 0, -- which is the last index used so far? New joysticks will be assigned the next one.
	joysticksByIndex = {}, -- access Joysticks by user Index - may contain non-connected joysticks, so test for that
	joysticksByGUID = {}, -- access Joysticks by GUID - only contains connected joysticks
}

function JoystickManager:GetIndex(GUID)
	local joy = self.joysticksByGUID[GUID]
	if joy then
		return joy.index
	else
		return -1
	end
end

-- reads saved indices from config file
local function JoystickManager_Load(self)
	-- function called from config file
	local function CreateJoystick(info)
		local joy = Joystick:New(info)
		-- check for invalid entries, remove them.
		if joy.index == -1 then
			jar.Logger.GetDefaultLogger():Warning("Config file contained Joystick \"".. joy.name .."\" without valid index, removing entry.")
			return
		elseif joy.name == "" then
			jar.Logger.GetDefaultLogger():Warning("Config file contained Joystick without valid name, removing entry.")
			return
		elseif joy.GUID == "" then
			jar.Logger.GetDefaultLogger():Warning("Config file contained Joystick without valid ID, removing entry.")
			return
		elseif self.joysticksByIndex[joy.v] then
			jar.Logger.GetDefaultLogger():Warning("Config file contained duplicate Joystick index, removing entry.")
			return
		end
		-- safe valid entry
		self.joysticksByIndex[joy.index] = joy
		self.lastIndex = math.max(self.lastIndex, joy.index)
	end
	--todo (really? or is this just some leftover code? in any case, I should re-evaluate the way I handle joystick input)
	local code, err = loadfile(self.configfilename)
	if not code then
		jar.Logger.GetDefaultLogger():Info("Could not load joystick config file " .. self.configfilename ..": " .. err, 0)
		return false
	end
	-- create secure function environment
	local functionEnvironment =
	{
		Joystick = CreateJoystick,
	}
	setfenv(code, functionEnvironment)
	-- try and execute file
	local success, err = pcall(code)
	if not success then
		-- caught an error
		jar.Logger.GetDefaultLogger():Warning(self.configfilename .. " contains an error: " .. err)
		return false
	end
	return true
end

local function JoystickManager_ProcessAvailableJoysticks(self)
	local connectedIDJs = {} -- by GUID
	-- see which Joysticks are connected
	for idj in jar.Input.GetSingleton():GetInputDeviceManager():GetAllControllers() do
		if connectedIDJs[idj:GetUniqueID()] then
			jar.Logger.GetDefaultLogger():Warning("Duplicate Joystick GUID \"" .. idj:GetUniqueID() .. "\"! (" .. idj:GetName() .. ") Disabling duplicate joystick.")
		else
			connectedIDJs[idj:GetUniqueID()] = idj
		end
	end
	-- see which of them are saved and thus have an assigned index
	for index, joystick in pairs(self.joysticksByIndex) do
		assert(index == joystick.index) -- just checkin once more. These ought to be in sync.
		if connectedIDJs[joystick.GUID] then -- hey, this joystick is connected!
			joystick:SetFromIDJ(connectedIDJs[joystick.GUID])
			connectedIDJs[joystick.GUID] = nil
			self.joysticksByGUID[joystick.GUID] = joystick
		else -- this joystick is not connected.
			-- oh well, not much to do then
			jar.Logger.GetDefaultLogger():Info("Joystick \"" .. joystick.name .. "\" not connected anymore.", 3)
		end
	end
	-- the rest needs to be assigned a new v.
	for _, idj in pairs(connectedIDJs) do
		local joy = Joystick:New()
		joy:SetFromIDJ(idj)
		self.lastIndex = self.lastIndex + 1
		joy.index = self.lastIndex
		self.joysticksByGUID[joy.GUID] = joy
		self.joysticksByIndex[joy.index] = joy
	end
	return true
end

--! \brief Initializes the manager by loading the configfile and checking the actual available joysticks
--! \return success
function JoystickManager:Init()
	JoystickManager_Load(self) -- not bad if that fails, most likely means the file didn't exist
	if not JoystickManager_ProcessAvailableJoysticks(self) then
		return false
	end
	return true
end

--! \brief Saves the settings to the configfile
function JoystickManager:Save()
	local file = jar.fs.OpenWrite(self.configfilename)
	if not file then
		jar.Logger.GetDefaultLogger():Error("Could not write file " .. self.configfilename .."! " .. jar.fs.GetLastError())
		return false
	end
	file:WriteString("-- automatically generated joystick config. Changes to the layout and comments will be discarded on next save.\n")

	for _, joystick in pairs(self.joysticksByIndex) do
		file:WriteString(joystick:ToSaveString())
	end
	file:Close()
	return true
end

-- todo: functions for user setup (they don't want to fiddle with config files)