require("InstructionInterpreter.lua")
require("CCommandManager.lua")
require("MainMenuState.lua")

g_running = true
g_CCommandManager:RegisterCommand
{
	name = "quit",
	description = "Exits the program",
	Execute = function(self)
		g_running = false
	end,
}

g_currentState = MainMenuState

local hasFocus = true
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
			if g_currentState.GainFocus then g_currentState:GainFocus() end
			--g_Window:ShowMouseCursor(false)
		elseif event.Type == jar.Event.LostFocus then
			hasFocus = false
			if g_currentState.LoseFocus then g_currentState:LoseFocus() end
			--g_Window:ShowMouseCursor(true)
		elseif hasFocus then
			if not g_EventListenerStack:OnEvent(event) then
				g_currentState:OnEvent(event)
			end
		end
	end
	g_EventListenerStack:PostEvent()
end

while g_running do
	HandleEvents()
	
	local frametime = g_Window:GetFrameTime()
	if frametime == 0 then -- I clamp the frame rate to a thousand fps because my time is in milliseconds.
		jar.Sleep(1)
		frametime = g_Window:GetFrameTime()
	end
	
	g_InstructionInterpreter:Update(frametime)
	jar.Core.GetSingleton():Update(frametime)
	g_currentState:Update(frametime)
	
	g_Window:Clear(jar.Color.Black)
	g_currentState:RenderTo(g_Window)
	--console last since it's an overlay
	g_Console:RenderTo(g_Window)
	
	g_Window:Display()
	if not hasFocus then
		-- sleep 5 ms
		jar.Sleep(5)
	end
end

g_CVarManager:SaveCVars()

print("Bye Bye!")