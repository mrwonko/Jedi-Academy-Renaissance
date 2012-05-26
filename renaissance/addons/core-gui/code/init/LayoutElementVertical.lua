require ("LayoutElement.lua")

LayoutElementVertical = LayoutElement:New()
LayoutElementVertical.classname = "VerticalLayout"

-- register to list of available layout elements
LayoutElement.Register(LayoutElementVertical)

-- FromTable does not need further specialization