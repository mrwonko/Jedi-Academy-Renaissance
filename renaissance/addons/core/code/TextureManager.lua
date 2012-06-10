class "TextureManager" (jar.TextureManager)

function TextureManager:__init()
	jar.TextureManager.__init(self)
end

function TextureManager:GetTexture(filename)
	print("TextureManager:GetTexture(" .. filename .. ")")
	return nil
end