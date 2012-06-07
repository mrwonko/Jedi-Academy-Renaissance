require ("LayoutElement.lua")

LayoutElementVertical = LayoutElement:New()
LayoutElementVertical.classname = "VerticalLayout"

-- register to list of available layout elements
LayoutElement.Register(LayoutElementVertical)

-- FromTable does not need further specialization

function LayoutElementVertical:OnSizeChange()
	local posY = 0
	local width = self.size.X
	local controls = self.controls
	table.sort(controls, function(lhs, rhs) return lhs.order < rhs.order end)
	for _, control in ipairs(controls) do
		local controlSize = control.size
		control:SetPosition(jar.Vector2f((width - controlSize.X) / 2, posY))
		posY = posY + controlSize.Y
	end
end