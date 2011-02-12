EventListenerStack = {}

function EventListenerStack:New()
	local obj =
	{
		eventListeners = {},
		inactive = false,
	}
	setmetatable(obj, self)
	self.__index = self
	return obj
end

--[[!	\brief Calls listeners' OnEvent() functions, which may return true to signal a desire for abortion, in which case it will call no later listener's OnEvent().
		\return true if any listeners aborted, false otherwise.
--]]
function EventListenerStack:OnEvent(event)
	for i=#self.eventListeners, 1, -1 do
		if self.eventListeners[i]:OnEvent(event) then
			return true
		end
	end
	return false
end

-- Called before event handling each frame
function EventListenerStack:PreEvent()
	for _, listener in ipairs(self.eventListeners) do
		if listener.PreEvent then
			listener:PreEvent()
		end
	end
end

-- Called after event handling each frame
function EventListenerStack:PostEvent()
	for _, listener in ipairs(self.eventListeners) do
		if listener.PostEvent then
			listener:PostEvent()
		end
	end
end

--[[!	\brief Pushes a new listener onto the stack if it's not already part of it
		\return Whether it was pushed, i.e. did not exist yet
--]]
function EventListenerStack:PushListener(listener)
	assert(type(listener.OnEvent) == "function")
	for _, l in ipairs(self.eventListeners) do
		if l == listener then
			return false
		end
	end
	table.insert(self.eventListeners, listener)
	return true
end

--[[!	\brief Removes a listener from the stack, if it is part of it
		\return Whether the listener existed
--]]
function EventListenerStack:RemoveListener(listener)
	for i, l in ipairs(self.eventListeners) do
		if l == listener then
			table.remove(self.eventListeners, i)
			return true
		end
	end
	return false
end

g_EventListenerStack = EventListenerStack:New()