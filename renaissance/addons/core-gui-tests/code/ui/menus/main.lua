Menu
{
	name = "main",
	layout = "main",
	Text
	{
		text = "Main Menu",
		container = "title", -- in which layout container this goes
		order = 100, -- for ordering in the container
		-- style information could go here - e.g. a "class", whose properties are set via style file
	},
	Button
	{
		text = "Buttön!",
		container = "mainmenu",
		order = 100,
	},
	Button
	{
		text = "Quit!",
		container = "mainmenu",
		order = 200,
	},
}