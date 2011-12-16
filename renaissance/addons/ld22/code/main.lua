require("InstructionInterpreter.lua")
require("CCommandManager.lua")

jar.Logger.GetDefaultLogger():Info("Welcome to my Ludum Dare 22 game! I see you have logging enabled... :)", 3)

-- global variable to make the game stop
g_running = true

-- register console command to quit the game ("quit")
g_CCommandManager:RegisterCommand
{
	name = "quit",
	description = "Exits the program",
	Execute = function(self)
		g_running = false
	end,
}

-- whether the windows is currently in focus
local hasFocus = true

-- Event handling function
local function HandleEvents()
	g_EventListenerStack:PreEvent()
	while true do
		local success, event = jar.EventManager.GetSingleton():GetEvent()
		if not success then
			-- no more events available
			break
		end
		
		if event.Type == jar.Event.Closed then
			g_running = false
		elseif event.Type == jar.Event.GainedFocus then
			hasFocus = true
			--g_Window:ShowMouseCursor(false)
		elseif event.Type == jar.Event.LostFocus then
			hasFocus = false
			--g_Window:ShowMouseCursor(true)
		elseif hasFocus then
			-- send input to listener stack (e.g. console?) - only process if not used there. (?)
			if not g_EventListenerStack:OnEvent(event) then
				-- process input here
			end
		end
	end
	g_EventListenerStack:PostEvent()
end

local slept = false
while g_running do
	HandleEvents()
	
	local frametime = g_Window:GetFrameTime()
	if frametime == 0 then -- I clamp the frame rate to a thousand fps because my time is in milliseconds.
		jar.Sleep(1)
		frametime = 1
	end
	if slept then
		frametime = frametime + 5 -- GetFrameTime seems to return the *actual* frame time, not the time since the last frame.
		slept = false
	end
	
	-- do instruction (i.e. console string) interpreting
	g_InstructionInterpreter:Update(frametime)
	-- update all Updateables (do I even use that? I think I do internally e.g. to poll joysticks or something...)
	jar.Core.GetSingleton():Update(frametime)
	-- updating game state goes here
	
	-- clear window
	g_Window:Clear(jar.Color.Black)
	
	-- stuff rendering goes here
	
	--console last since it's an overlay
	g_Console:RenderTo(g_Window)
	
	-- verify: is this true? (leftover code from LD20)
	--hack: there's some kind of bug related to a wrong view being used when the console is opened in the main menu. This circumvents it.
	-- g_Window:SetView(g_Window:GetDefaultView())
	
	g_Window:Display()
	
	-- sleep if out of focus
	if not hasFocus then
		-- sleep 5 ms
		jar.Sleep(5)
		slept = true
	end
end

-- save cvars on quit
g_CVarManager:SaveCVars()