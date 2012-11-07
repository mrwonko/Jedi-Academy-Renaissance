FontManager = {}

local weakMetatable =
{
	__mode = "v", --weak values
}

function FontManager:New()
	local obj =
	{
		fonts = {} -- this gets turned into a weak table, which means values aren't counted for reference counting, i.e. they will be deleted when there are no more other references
	}
	setmetatable(obj, self)
	setmetatable(obj.fonts, weakMetatable)
	self.__index = self
	return obj
end

--[[!	\brief Retrieves a font by filename ("fonts/" is optional)
		\return The font, or nil on error (font could not be loaded)
--]]
function FontManager:GetFont(fontname, throwError)
	-- has the font been loaded already?
	if self.fonts[fontname] then
		return self.fonts[fontname]
	elseif self.fonts["fonts/" .. fontname] then
		return self.fonts["fonts/" .. fontname]
	end
	-- load the font
	local font = jar.Font()
	if font:LoadFromFile(fontname) then
		self.fonts[fontname] = font
		return font
	elseif font:LoadFromFile("fonts/" .. fontname) then
		self.fonts["fonts/" .. fontname] = font
		return font
	end
	if throwError then
		error("Could not load font " .. fontname .. "!", 2)
	end
	-- return nil (done anyway)
end