--[[! ModVersionInfo contains information about the version of a mod. It is used for dependencies and incompatibilities.
--]]
ModVersionInfo = {}

function ModVersionInfo:New()
	local obj = { name = "" }
	setmetatable(obj, self)
	self.__index = self
	return obj
end

-- whether the given mod satisfies the "requirements"
function ModVersionInfo:Applies(mod)
	if not mod then return false end
	if mod.name ~= self.name then return false end
	if self.isCorrectVersion then
		assert(type(self.isCorrectVersion) == "function")
		return self.isCorrectVersion(mod.version)
	end
	if self.version then
		return mod.version == self.version
	end
	if self.minVersion and self.maxVersion then
		return mod.version <= self.maxVersion and mod.version >= self.minVersion
	end
	if self.minVersion then
		return mod.version >= self.minVersion
	end
	if self.maxVersion then
		return mod.version <= self.maxVersion
	end
	-- no version supplied -> any version
	return true
end

function ModVersionInfo:GetDisplayVersion()
	if self.displayName then return self.displayName end
	if self.isCorrectVersion then return end
	if self.version then return tostring(self.version) end
	if self.minVersion and self.maxVersion then return "minimum: " .. self.minVersion .. ", maximum: " .. self.maxVersion end
	if self.minVersion then return "minimum: "..self.minVersion end
	if self.maxVersion then return "maximum: "..self.maxVersion end
	return "any"
end

function ModVersionInfo:AppliesToAnyVersion()
	if self.minVersion or self.maxVersion or self.version or self.isCorrectVersion then
		return true
	end
	return false
end