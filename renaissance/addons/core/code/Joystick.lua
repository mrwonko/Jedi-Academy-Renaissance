require("StringUtils.lua")

Joystick =
{
}

function Joystick:New(info)
	if type(info) ~= "table" then
		info = {}
	end
	if info.GUID and type(info.GUID) ~= "string" then info.GUID = nil end
	if info.name and type(info.name) ~= "string" then info.name = nil end
	if info.sensitivity and type(info.sensitivity) ~= "number" then info.sensitivity = nil end
	if info.index then
		if type(info.index) ~= "number" or info.index < 1 then
			info.index = nil
		else
			info.index = math.floor(info.index)
		end
	end
	local obj =
	{
		GUID = info.GUID or "", -- unique identifier that stays the same between executions
		name = info.name or "", -- human readable name, may not be unique - saved in case of disconnection
		index = info.index or -1, -- users assign indices to the joysticks which are used in binds (e.g. JOY1_BUTTON3)
		sensitivity = info.sensitivity or 1,
		
		internalIndex = -1, -- index as used in the c++ lowlevel part of the engine, which may change between executions
		connected = false, -- whether the joystick is currently connected - non-connected joysticks get saved so their userIndices are retained.
	}
	if obj.sensitivity == 0 then -- negative is okay, means "flipped axis"
		obj.sensitivity = 1
	end
	setmetatable(obj, self)
	self.__index = self
	return obj
end

function Joystick:SetFromIDJ(idj) -- IDJ = jar.InputDeviceJoystick FIXME: renamed from Joystick to Controller - rename in lua everywhere, too!
	if self.GUID ~= "" then
		assert(self.GUID == idj:GetUniqueID())
	else
		self.GUID = idj:GetUniqueID()
	end
	if self.name ~= "" then
		if self.name ~= idj:GetName() then
			jar.Logger.GetDefaultLogger():Warning("Joystick name change detected! Old name: \"" .. self.name .. "\" New name: \"" .. idj:GetName() .. "\" This is most likely due to a bug. Please report.")
			self.name = idj:GetName()
		end
	else
		self.name = idj:GetName()
	end
	self.connected = true
	self.internalIndex = idj:GetIndex()
end

function Joystick:ToSaveString(functionName)
	return (functionname or "Joystick") .. "\n\
{\n\
\tname = \"" .. Escape(self.name) .. "\",\n\
\tGUID = \"" .. Escape(self.GUID) .. "\",\n\
\tindex = " .. self.index .. ",\n\
\tsensitivity = " .. self.sensitivity .. ",\n\
}\n"
end

-- Applies sensitivy. Dead zone cannot be done here since it depends on two axes - now this is really confusing. I'll need to know which axes belong together? TODO: Need more research.
function Joystick:ApplySensitivity(value)
	return value * self.sensitivity;
end