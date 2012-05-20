== GUI System ==

Gui scripts go into code/init/hud and code/init/menu - they're lua scripts and as such end in .lua, and while there are some differences between menus and hud elements, there are similarities as well.

== Menu ==

Here's all that can go into a menu file:

```Lua
Menu -- this means: a menu starts now!
{
	name = "my cool menu", -- menu name - mandatory!
	PrecacheSound -- sound files to be available while this menu is open
	{
		"sound/my/file.wav",
	},
	itemFocusSound = "my/sound/file.wav", -- sound to play when (selectable) items get focused
	itemFocusForce = "rumble/focus", -- force feedback effect to play when items get focused
	
	Button
	{
		OnClick = function(menu, button) end, -- called when the button is clicked - i.e. pressed, and then released with the mouse still on the button
		OnPress = function(menu, button) end, -- called when the button is pressed
		OnRelease = function(menu, button) end, -- called when the button is released
	},
}
```

== Common Menu Items ==

== Common Item properties ==

All Menu Items have these properties:

```Lua
OnMouseOver = function(menu, item) end, -- called when the mouse enters the element
OnMouseOut = function(menu, item) end, -- called when the mouse leaves the element
```