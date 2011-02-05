Console =
{
	isOpened = false,
	-- treat the below private --
	
	mInput = "",
	-- keys to open/close the console
	mConsoleKeys =
	{
		[jar.Key.BackSlash] = true,
		[jar.Key.Tilde] = true,
	},
	mInterpreter = g_InstructionInterpreter
}

function Console:New(width, height, interpreter, font)
	if type(width) ~= "number" then
		error("Console:New(): width must be a number!", 2)
	end
	if type(height) ~= "number" then
		error("Console:New(): height must be a number!", 2)
	end
	if not font then
		error("Console:New(): No font supplied!", 2)
	end
	
	local obj =
	{
		mSize = jar.Vector2f(width, height),
		mHistory = {},
		mRect = jar.Shape.Rectangle(0, 0, width, height-1, jar.Color(0, 0, 0, 195), 1, jar.Color.White),
		mInterpreter = interpreter,
		mFont = font,
	}
	setmetatable(obj, self)
	self.__index = self
	if not obj.mInterpreter then
		error("Console:New(): No valid interpreter set!", 2)
	end
	return obj
end

function Console:OnEvent(event)
	if event.Type ~= jar.Event.KeyPressed then
		return false
	end
	if self.mConsoleKeys[event.Key.Code] and event.Key.Shift then
		self.isOpened = not self.isOpened
		print("Console " .. (self.isOpened and "opened" or "closed"))
		return true
	end
	if not self.isOpened then
		return false
	end
end

function Console:RenderTo(target)
	if not self.isOpened then
		return
	end
	local oldView = target:GetView()
	target:SetView(jar.View(jar.Vector2f(self.mSize.x/2, self.mSize.y), jar.Vector2f(self.mSize.x/2, self.mSize.y)))
	target:Draw(self.mRect)
	target:SetView(oldView)
end