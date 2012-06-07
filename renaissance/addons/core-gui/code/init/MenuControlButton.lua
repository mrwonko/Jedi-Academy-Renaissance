require ("MenuControl.lua")
require ("FontManager.lua")

g_fontManager = g_fontManager or FontManager:New()
local font = g_fontManager:GetFont("times") or error("Could not find \"times\" font!")


local margin = 1 -- outside the border
local border = 1 -- border width
local padding = 1 -- inside the border
local marginVec = jar.Vector2f(margin, margin)
local borderVec = jar.Vector2f(border, border)
local paddingVec = jar.Vector2f(padding, padding)

local function count(str, pattern)
	local count = 0
	for _ in string.gmatch(str, substr) do
		count = count + 1
	end
	return count
end

MenuControlButton = MenuControl:New()
local MenuControlButton = MenuControlButton
MenuControlButton.classname = "Button"

-- register to list of available layout elements
MenuControl.Register(MenuControlButton)

-- FromTable does not need further specialization
function MenuControlButton:FromTable(t)
	MenuControl.FromTable(self, t)
	if not self.text then error("No text string set!", 3) end
	self.drawableText = jar.Text(font)
	self.drawableText:SetText(self.text)
	local size = jar.Vector2f(self.drawableText:GetWidth(), self.drawableText:GetHeight())
	self.size = size + (marginVec + borderVec + paddingVec) * 2
	self.drawableBorder = jar.RectangleShape(size + paddingVec * 2)
	self.drawableBorder:SetFillColor(jar.Color.Transparent)
	self.drawableBorder:SetOutlineColor(jar.Color.White)
	self.drawableBorder:SetOutlineThickness(border)
end

function MenuControlButton:Draw(target)
	target:Draw(self.drawableBorder)
	target:Draw(self.drawableText)
end

do
	function MenuControlButton:SetPosition(pos)
		self.drawableText:SetPosition(pos + marginVec + borderVec + marginVec)
		self.drawableBorder:SetPosition(pos + marginVec + borderVec)
	end
end