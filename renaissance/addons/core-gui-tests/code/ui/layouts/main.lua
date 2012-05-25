-- main menu layout:
-- title on top, buttons below

local margin = width * 0.05
local titleHeight = 100
local menuWidth = 400

Layout
{
	name = "main",
	-- title
	VerticalLayout
	{
		name = "title",
		position = { margin, margin },
		size = { width - 2 * margin, titleHeight },
		-- style "class" could go here (e.g.: border, bg color, bg image)
	},
	-- main menu buttons
	VerticalLayout
	{
		name = "mainmenu",
		position = { width / 2 - menuWidth / 2, titleHeight + 2 * margin },
		size = { menuWidth, height - 3 * margin - titleHeight },
	},
}

-- duplicate testing
Layout
{
	name = "main",
}