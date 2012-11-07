ImageManager =
{
	--TODO: make an ImageManager.errorimage!
	errorimage = nil
}

local weakMetatable =
{
	__mode = "v", --weak values
}

function ImageManager:New()
	local obj =
	{
		images = {} -- this gets turned into a weak table, which means values aren't counted for reference counting, i.e. they will be deleted when there are no more other references
	}
	setmetatable(obj, self)
	setmetatable(obj.images, weakMetatable)
	self.__index = self
	return obj
end

--[[!	\brief Retrieves an image by filename ("textures/" is NOT optional)
		\return The image, or nil on error (image could not be loaded, i.e. (probably) not found)
--]]
function ImageManager:GetImage(imgname, throwError)
	if type(imgname) ~= "string" then
		if DEBUG then
			error("ImageManager:GetImage() called with invalid argument: no string!")
		else
			jar.Logger.GetDefaultLogger():Warning("ImageManager:GetImage() called with invalid argument: no string!")
			return
		end
	end
	
	-- make sure we have no extension, the same image may be loaded multiple times otherwise (once with and once without extension)
	if imgname:sub(-4) == ".png" then
		imgname = imgname:sub(1, -5)
		--print("stripped imgname: " .. imgname)
	end
	if imgname:sub(-4) == ".tga" then
		imgname = imgname:sub(1, -5)
		--print("stripped imgname: " .. imgname)
	end
	if imgname:sub(-4) == ".jpg" then
		imgname = imgname:sub(1, -5)
		--print("stripped imgname: " .. imgname)
	end
	
	-- has the image been loaded already?
	if self.images[imgname] then
		return self.images[imgname]
	end
	
	-- not loaded yet, load it!
	local img = jar.Texture()
	if img:LoadFromFile(imgname) then
		self.images[imgname] = img
		return img
	end
	-- leave warning to user, so a more specific location can be added.
	if throwError then
		error("Could not load image " .. imgname .. "!", 2)
	end
	return self.errorimage
end