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
	g_Music:SetVolume(g_CVarManager:GetCVarValue("snd_musicvolume") or 100)
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
			--g_Window:SetMouseCursorVisible(false)
		elseif event.Type == jar.Event.LostFocus then
			hasFocus = false
			if currentState.LoseFocus then currentState:LoseFocus() end
			--g_Window:SetMouseCursorVisible(true)
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

local lastFrametime = jar.GetTime()
while g_running do
	HandleEvents()
	
	local frametime = jar.GetTime()
	if frametime == lastFrametime then -- I clamp the frame rate to a thousand fps because my time is in milliseconds.
		jar.Sleep(1)
		frametime = jar.GetTime()
	end
	local deltaT = frametime - lastFrametime
	assert( deltaT >= 0)
	lastFrametime = frametime
	
	g_InstructionInterpreter:Update(deltaT)
	jar.Core.GetSingleton():Update(deltaT)
	currentState:Update(deltaT)
	
	g_Window:Clear(jar.Color.Black)
	currentState:RenderTo(g_Window)
	--console last since it's an overlay
	g_Console:RenderTo(g_Window)
	--hack: there's some kind of bug related to a wrong view being used when the console is opened in the main menu. This circumvents it.
	g_Window:SetView(g_Window:GetDefaultView())
	
	g_Window:Display()
	if not hasFocus then
		-- sleep 5 ms
		jar.Sleep(5)
	end
end

g_CVarManager:SaveCVars()

print("Bye Bye!")