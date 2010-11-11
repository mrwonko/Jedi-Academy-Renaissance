name = "Wonko's BFG Mod"
version = 3
displayVersion = "1.2" -- the displayed version is not necessarily the internal version!
author = "Mr. Wonko"
description = "Adds a big, uh, freaking gun that goes BOOM."
website = "http://www.mrwonko.de"
email = "spam@mrwonko.de"
dependencies =
{
	-- I need stuff from my effect library
	{
		name = "Wonko's new effects",
		minVersion = 5
	}
}
incompatibilities =
{
	-- John uses the same name for his BFG so they overwrite each other -> incompatible
	{
		name = "John's BFG Mod"
	}
}
