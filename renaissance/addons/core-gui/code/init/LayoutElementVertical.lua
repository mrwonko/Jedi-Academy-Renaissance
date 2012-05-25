require ("LayoutElement.lua")

LayoutElementVertical = LayoutElement:New()
LayoutElementVertical.classname = "VerticalLayout"

-- register to list of available layout elements
LayoutElement.Register(LayoutElementVertical)

function LayoutElementVertical:FromTable(t)
	if not LayoutElement.FromTable(self, t) then
		return false
	end
	-- todo (?)
	return true
end