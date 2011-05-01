require("InstructionInterpreter.lua")
require("CCommandManager.lua")
require("MainMenuState.lua")
require("KeyManager.lua")

g_running = true
g_CCommandManager:RegisterCommand
{
	name = "quit",
	description = "Exits the program",
	Execute = function(self)
		g_running = false
	end,
}

local currentState = false
function SetCurrentState(state)
	if not state then
		return false
	end
	if currentState and currentState.OnEnd then
		currentState:OnEnd()
	end
	if state.OnStart then
		state:OnStart()
	end
	currentState = state
end
g_Music = jar.Sound()
g_Music:SetLoop(true)
local buf = g_SoundManager:GetSound("sound/music.ogg")
if buf then
	g_Music:SetBuffer(buf)
	g_Music:SetVolume(g_CVarManager:GetCVar("snd_musicvolume") or 100)
	g_Music:Play()
end

SetCurrentState(MainMenuState)

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
			if currentState.GainFocus then currentState:GainFocus() end
			--g_Window:ShowMouseCursor(false)
		elseif event.Type == jar.Event.LostFocus then
			hasFocus = false
			if currentState.LoseFocus then currentState:LoseFocus() end
			--g_Window:ShowMouseCursor(true)
		elseif hasFocus then
			if event.Type == jar.Event.KeyPressed then
				KeyManager:OnKeyDown(event.Key.Code)
			elseif event.Type == jar.Event.KeyReleased then
				KeyManager:OnKeyUp(event.Key.Code)
			end
			if not g_EventListenerStack:OnEvent(event) then
				currentState:OnEvent(event)
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
	
	g_InstructionInterpreter:Update(frametime)
	jar.Core.GetSingleton():Update(frametime)
	currentState:Update(frametime)
	
	g_Window:Clear(jar.Color.Black)
	currentState:RenderTo(g_Window)
	--console last since it's an overlay
	g_Console:RenderTo(g_Window)
	
	g_Window:Display()
	if not hasFocus then
		-- sleep 5 ms
		jar.Sleep(5)
		slept = true
	end
end

g_CVarManager:SaveCVars()

print("Bye Bye!")