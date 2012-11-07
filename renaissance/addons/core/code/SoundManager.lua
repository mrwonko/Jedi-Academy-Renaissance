SoundManager =
{
}

local weakMetatable =
{
	__mode = "v", --weak values
}

function SoundManager:New()
	local obj =
	{
		sounds = {} -- this gets turned into a weak table, which means entries aren't counted for reference counting, i.e. they will be deleted when there are no more other references
	}
	setmetatable(obj, self)
	setmetatable(obj.sounds, weakMetatable)
	self.__index = self
	return obj
end

--[[!	\brief Retrieves a sound by filename ("sound/" is NOT optional)
		\return The sound, or nil on error (sound could not be loaded, i.e. (probably) not found)
--]]
function SoundManager:GetSound(name, throwError)
	if type(name) ~= "string" then
		if DEBUG then
			error("SoundManager:GetSound() called with invalid argument: no string!")
		else
			jar.Logger.GetDefaultLogger():Warning("SoundManager:GetSound() called with invalid argument: no string!")
			return
		end
	end
	
	-- make sure we have no extension, the same sound may be loaded multiple times otherwise (once with and once without extension)
	if name:sub(-4) == ".wav" then
		name = name:sub(1, -5)
	end
	if name:sub(-4) == ".mp3" then
		name = name:sub(1, -5)
	end
	if name:sub(-4) == ".ogg" then
		name = name:sub(1, -5)
	end
	
	-- has the sound been loaded already?
	if self.sounds[name] then
		return self.sounds[name]
	end
	
	-- not loaded yet, load it!
	local sound = jar.SoundBuffer()
	if sound:LoadFromFile(name) then
		self.sounds[name] = sound
		return sound
	end
	-- leave warning to user, so a more specific location can be added.
	if throwError then
		error("Could not load sound " .. name .. "!", 2)
	end
	return
end