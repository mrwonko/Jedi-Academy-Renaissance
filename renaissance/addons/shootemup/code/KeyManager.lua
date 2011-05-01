KeyManager =
{
	pressedKeys = {},
}

function KeyManager:OnKeyDown(key)
	self.pressedKeys[key] = true
end

function KeyManager:OnKeyUp(key)
	self.pressedKeys[key] = nil
end

function KeyManager:IsKeyDown(key)
	return self.pressedKeys[key] or false
end