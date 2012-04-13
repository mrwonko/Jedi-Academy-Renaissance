require("IntroState.lua")

MainMenuState =
{
}

function MainMenuState:OnEvent(event)
	if event.Type == jar.Event.KeyPressed then
		if event.Key.Code == jar.Key.Escape then
			g_running = false
			return true
		elseif event.Key.Code == jar.Key.Return then
			SetCurrentState(IntroState)
			return true
		end
	end
	return false --has not been used
end

function MainMenuState:Update(deltaT)
end

local arial = g_FontManager:GetFont("arial", true)

local title = jar.Text(arial, 20) -- second parameter: throw error on failure
title:SetText("Space Danger")
title:SetPosition( (g_Window:GetSize().X - title:GetWidth()) / 2, 20)

local title2 = jar.Text(arial, 18)
title2:SetText("by Mr. Wonko")
title2:SetPosition( (g_Window:GetSize().X - title2:GetWidth()) / 2, title:GetPosition().Y + title:GetHeight())

local description = jar.Text(arial)
local wrappedDescriptionText = table.concat(WordWrap([[A Ludum Dare 20 entry, created in 48 hours. (No more than 36 hours actually, since the first idea was eventually scratched.)

Move and select weapon using the arrow keys, shoot with space.

You can open the console using shift + ~/ö and set stuff there, for example the resolution (r_width/r_height/r_fullscreen/r_borderless) and key mapping (key_***), just check out cvarlist and help.


Press Enter to start.


Oh, and in case you want to exit: Press escape now!]], arial, nil, g_Window:GetSize().X-20), "\n")
-- one empty line after title2
description:SetPosition(10, title2:GetPosition().Y + title2:GetHeight() + title2:GetLineHeight())
description:SetText(wrappedDescriptionText)

function MainMenuState:RenderTo(target)
	target:Draw(title)
	target:Draw(title2)
	target:Draw(description)
end

function MainMenuState:OnStart()
	--friggin wolfram tones terms of use
	--local buf = g_SoundManager:GetSound("sound/mus_title.ogg")
	--gotta make my own -.-
	--which is now in Main().
end