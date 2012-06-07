require ("MenuControl.lua")
require ("FontManager.lua")

g_fontManager = g_fontManager or FontManager:New()
local font = g_fontManager:GetFont("times") or error("Could not find \"times\" font!")

local function count(str, pattern)
	local count = 0
	for _ in string.gmatch(str, substr) do
		count = count + 1
	end
	return count
end

MenuControlText = MenuControl:New()
local MenuControlText = MenuControlText
MenuControlText.classname = "Text"

-- register to list of available layout elements
MenuControl.Register(MenuControlText)

-- FromTable does not need further specialization
function MenuControlText:FromTable(t)
	MenuControl.FromTable(self, t)
	if not self.text then error("No text string set!", 3) end
	self.drawableText = jar.Text(font)
	self.drawableText:SetText(self.text)
	self.size = jar.Vector2f(self.drawableText:GetWidth(), self.drawableText:GetHeight())
end

function MenuControlText:Draw(target)
	target:Draw(self.drawableText)
end

function MenuControlText:SetPosition(pos)
	self.drawableText:SetPosition(pos)
end