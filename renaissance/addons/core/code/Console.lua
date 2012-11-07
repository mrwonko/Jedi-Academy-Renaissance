require("CVarManager.lua")
require("StringUtils.lua")
require("WordWrap.lua")
require("KeyboardLayouts.lua")
require("InstructionInterpreter.lua")
require("AutoComplete.lua")

-- these don't have OnChange since their value is simply read by the console whenever it's needed
g_CVarManager:RegisterCVar{
	name = "con_outputHistorySize",
	description = "Number of lines of console output to save, -1 means no limit",
	type = CVar.TYPES.INT,
	defaultValue = 100,
}
g_CVarManager:RegisterCVar{
	name = "con_inputHistorySize",
	description = "How much console input to save, -1 means no limit",
	type = CVar.TYPES.INT,
	defaultValue = 100,
}

Console =
{
	isOpened = false,
	-- treat the below private --
	
	mInput = "",
	-- keys to open/close the console
	mConsoleKeys =
	{
		['~'] = true,
		['^'] = true,
	},
	mInterpreter = g_InstructionInterpreter,
	mOutputOffset = 0,
	mCurrentInput = "", -- to save the latest input when scrolling through the input history
	mInputHistoryPos = 0, --current position in the input history; 0 means new input (i.e. mCurrentInput)
	mInputCursorPos = 1,
}

function Console:New(width, height, interpreter, font, fontsize)
	if type(width) ~= "number" then
		error("Console:New(): width must be a number!", 2)
	end
	if type(height) ~= "number" then
		error("Console:New(): height must be a number!", 2)
	end
	if not font then
		error("Console:New(): No font supplied!", 2)
	end
	fontsize = fontsize or 8
	
	local obj =
	{
		mSize = jar.Vector2f(width, height),
		mInputHistory = {}, --what's been drawn to the console recently
		mOutputHistory = {}, --stuff the user has recently entered
		mRect = jar.RectangleShape(jar.Vector2f(width, height-1)),
		mInterpreter = interpreter,
		mOutputText = jar.Text(font, fontsize),
		mInputText = jar.Text(font, fontsize),
	}
	obj.mRect:SetFillColor(jar.Color(0, 0, 0, 195))
	obj.mRect:SetOutlineThickness(1)
	obj.mRect:SetOutlineColor(jar.Color.White)
	setmetatable(obj, self)
	
	self.__index = self
	if not obj.mInterpreter then
		error("Console:New(): No valid interpreter set!", 2)
	end
	
	local numLines = (height-3) \ obj.mOutputText:GetLineHeight() -- \ is div (floored division)
	obj.mNumOutputLines = numLines - 1
	obj.mOutputText:SetPosition(0, 2)
	obj.mInputText:SetPosition(0, 2 + (numLines - 1) * obj.mOutputText:GetLineHeight())
	obj.mInputText:SetText("]_")
	
	return obj
end

function Console:OnEvent(event)
	if event.Type ~= jar.Event.KeyPressed then
		return false
	end
	if self.mConsoleKeys[KeycodeToChar(event.Key.Code)] and event.Key.Shift then
		self.isOpened = not self.isOpened
		return true
	end
	if self.isOpened and event.Key.Code == jar.Key.Escape then
		self.isOpened = false
		return true
	end
	if not self.isOpened then
		return false
	end
	--scrolling the output history
	if event.Key.Code == jar.Key.PageUp then
		if self.mOutputOffset < (#self.mOutputHistory - self.mNumOutputLines) then
			self.mOutputOffset = self.mOutputOffset + 1
			self:UpdateOutputText()
		end
		return true
	end
	if event.Key.Code == jar.Key.PageDown then
		if self.mOutputOffset > 0 then
			self.mOutputOffset = self.mOutputOffset - 1
			self:UpdateOutputText()
		end
		return true
	end
	--selecting from the input history
	if event.Key.Code == jar.Key.Up or event.Key.Code == jar.Key.Down then
		if event.Key.Code == jar.Key.Up then
			if self.mInputHistoryPos < #self.mInputHistory then
				self.mInputHistoryPos = self.mInputHistoryPos + 1
			end
		elseif self.mInputHistoryPos > 0 then
			self.mInputHistoryPos = self.mInputHistoryPos - 1
		end
		if self.mInputHistoryPos > 0 then		
			assert(self.mInputHistory[self.mInputHistoryPos])
			self.mInputCursorPos = #self.mInputHistory[self.mInputHistoryPos] + 1
		else
			self.mInputHistoryPos = 0
			self.mInputCursorPos = #self.mCurrentInput + 1
		end
		self.mInputHasChanged = true
		return true
	end
	--moving cursor
	if event.Key.Code == jar.Key.Right or event.Key.Code == jar.Key.Left then
		local text = (self.mInputHistoryPos == 0) and self.mCurrentInput or self.mInputHistory[self.mInputHistoryPos]
		assert(text)
		local stop =
		{
			[" "] = true,
			["."] = true,
		}
		if event.Key.Code == jar.Key.Right then
			if self.mInputCursorPos <= #text then
				self.mInputCursorPos = self.mInputCursorPos + 1
				self.mInputHasChanged = true
				if event.Key.Control then
					while self.mInputCursorPos <= #text and not stop[text:sub(self.mInputCursorPos, self.mInputCursorPos)] do
						self.mInputCursorPos = self.mInputCursorPos + 1
					end
				end
			end
		else
			if self.mInputCursorPos > 1 then
				self.mInputCursorPos = self.mInputCursorPos - 1
				self.mInputHasChanged = true
				if event.Key.Control then
					while self.mInputCursorPos > 1 and not stop[text:sub(self.mInputCursorPos, self.mInputCursorPos)] do
						self.mInputCursorPos = self.mInputCursorPos - 1
					end
				end
			end
		end
		return true
	end
	if event.Key.Code == jar.Key.End then
		local text = (self.mInputHistoryPos == 0) and self.mCurrentInput or self.mInputHistory[self.mInputHistoryPos]
		self.mInputCursorPos = #text + 1
		self.mInputHasChanged = true
		return true
	end
	if event.Key.Code == jar.Key.Home then
		self.mInputCursorPos = 1
		self.mInputHasChanged = true
		return true
	end
	--sending text
	if event.Key.Code == jar.Key.Return then
		self:ProcessInput()
		self.mOutputOffset = 0
		self.mOutputHasChanged = true
		return true
	end
	--auto complete
	if event.Key.Code == jar.Key.Tab then
		--TODO: Parameter auto completion!
		local text = (self.mInputHistoryPos == 0) and self.mCurrentInput or self.mInputHistory[self.mInputHistoryPos]
		self.mInputHistoryPos = 0
		local avail = 0
		self.mCurrentInput, avail = self:AutoComplete(text)
		if #avail > 1 then
			print("]" .. self.mCurrentInput)
			for _, name in ipairs(avail) do
				print(name)
			end
		end
		self.mInputCursorPos = #self.mCurrentInput + 1
		self.mInputHasChanged = true
		return true
	end
	--deleting
	if event.Key.Code == jar.Key.Back then
		if self.mInputHistoryPos ~= 0 then
			self.mCurrentInput = self.mInputHistory[self.mInputHistoryPos] or ""
			self.mInputHistoryPos = 0
		end
		if self.mInputCursorPos > 1 then
			self.mCurrentInput = self.mCurrentInput:sub(1, self.mInputCursorPos-2) .. self.mCurrentInput:sub(self.mInputCursorPos)
			self.mInputCursorPos = self.mInputCursorPos - 1
			while event.Key.Control and self.mCurrentInput:sub(self.mInputCursorPos-1, self.mInputCursorPos-1) ~= " " and self.mCurrentInput ~= "" do
				self.mCurrentInput = self.mCurrentInput:sub(1, self.mInputCursorPos-2) .. self.mCurrentInput:sub(self.mInputCursorPos)
				self.mInputCursorPos = self.mInputCursorPos - 1
			end
			self.mInputHasChanged = true
		end
		return true
	end
	if event.Key.Code == jar.Key.Delete then
		if self.mInputHistoryPos ~= 0 then
			self.mCurrentInput = self.mInputHistory[self.mInputHistoryPos] or ""
			self.mInputHistoryPos = 0
		end
		self.mCurrentInput = self.mCurrentInput:sub(1, self.mInputCursorPos-1) .. self.mCurrentInput:sub(self.mInputCursorPos+1)
		self.mInputHasChanged = true
		return true
	end
	--entering text
	local char = KeycodeToChar(event.Key.Code, event.Key.Shift)
	if char then
		if self.mInputHistoryPos ~= 0 then -- already got custom input
			self.mCurrentInput = self.mInputHistory[self.mInputHistoryPos] or ""
			self.mInputHistoryPos = 0
		end
		self.mCurrentInput = self.mCurrentInput:sub(1, self.mInputCursorPos-1) .. char .. self.mCurrentInput:sub(self.mInputCursorPos)
		self.mInputCursorPos = self.mInputCursorPos + 1
		self.mInputHasChanged = true
		return true
	end
	return false
end

-- this is a little complicated since I want print to return to its old functionality once the console gets garbage collected.
local weakMetatable = { __mode = "v" }
local weakConsoleStorage = {}
oldPrint = print
setmetatable(weakConsoleStorage, weakMetatable)
local function ConsolePrint(...)
	assert(oldPrint)
	if not weakConsoleStorage.console then -- got destroyed?
		print = oldPrint
	else
		weakConsoleStorage.console:Print(...)
	end
	return oldPrint(...)
end

-- makes print() print to this console (as long as it lives)
function Console:BindPrint()
	weakConsoleStorage.console = self
	assert(oldPrint ~= ConsolePrint)
	oldPrint = print
	print = ConsolePrint
end

function Console:Print(...)
	local argStr = table.concat({...}, " ") .. "^7"
	local wrapped = WordWrap(argStr, self.mOutputText:GetFont(), self.mOutputText:GetFontSize(), self.mSize.X)
	
	--first:concatenate all arguments
	--next: calculate word wrap (result: array of lines)
	--last: send result to AddOutputText
	self:AddOutputLines(wrapped, "\n")
end

function Console:AddOutputLines(lines)
	local oldsize = #self.mOutputHistory
	for _, line in ipairs(lines) do
		table.insert(self.mOutputHistory, line)
	end
	--increase offset
	if self.mOutputOffset > 0 then
		self.mOutputOffset = self.mOutputOffset + #self.mOutputHistory - oldsize
	end
	
	local outputHistorySize = self.mInterpreter.cvarManager:GetCVarValue("con_outputHistorySize")
	assert(outputHistorySize)
	if outputHistorySize > 0 then
		for i = 1, #self.mOutputHistory - outputHistorySize do
			table.remove(self.mOutputHistory, 1)
		end
		--is the offset past the maximum?
		if outputHistorySize < self.mOutputOffset + self.mNumOutputLines then
			--lower it, then
			self.mOutputOffset = outputHistorySize - self.mNumOutputLines
		end
	end
	self.mOutputHasChanged = true
end

function Console:UpdateOutputText()
	self.mOutputHasChanged = nil
	-- trivial: enough space for everything
	if #self.mOutputHistory < self.mNumOutputLines then
		self.mOutputText:SetText(table.concat(self.mOutputHistory, "\n"))
		return
	end
	local text = {}
	local offset = #self.mOutputHistory - self.mNumOutputLines - self.mOutputOffset + 1
	for i = offset, self.mNumOutputLines + offset - 1 do
		assert(self.mOutputHistory[i])
		table.insert(text, self.mOutputHistory[i])
		table.insert(text, "\n")
	end
	self.mOutputText:SetText(table.concat(text))
end

function Console:UpdateInputText()
	self.mInputHasChanged = nil
	assert(self.mInputHistoryPos >= 0)
	assert(self.mInputHistoryPos <= #self.mInputHistory)
	local text = ""
	if self.mInputHistoryPos == 0 then
		text = self.mCurrentInput
	else
		text = self.mInputHistory[self.mInputHistoryPos]
	end
	local text = text:sub(1, self.mInputCursorPos-1) .. "_" .. text:sub(self.mInputCursorPos)
	--check if it goes out of the screen
	local font = self.mInputText:GetFont()
	local factor = self.mInputText:GetFontSize() / font:GetDefaultSize()
	local numCutFront = 0
	while font:GetWidth("]" .. text) * factor > self.mSize.X and #text > 5 do
		if numCutFront + 1 == self.mInputCursorPos then
			text = text:sub(1, -2)
		else
			text = text:sub(2)
			numCutFront = numCutFront + 1
		end
	end
	self.mInputText:SetText("]" .. text)
end

function Console:ProcessInput()
	if self.mInputHistoryPos ~= 0 then
		self.mCurrentInput = self.mInputHistory[self.mInputHistoryPos] or ""
		self.mInputHistoryPos = 0
	end
	if self.mCurrentInput == "" then
		return
	end
	local inst = self:AutoComplete(self.mCurrentInput)
	print("]" .. inst)
	self.mInterpreter:Interpret(inst)
	table.insert(self.mInputHistory, 1, inst)
	local historySize = self.mInterpreter.cvarManager:GetCVarValue("con_inputHistorySize")
	assert(historySize)
	while #self.mInputHistory > historySize and historySize ~= -1 do
		table.remove(self.mInputHistory)
	end
	self.mCurrentInput = ""
	self.mInputCursorPos = 1
	self.mInputHasChanged = true
end

function Console:AutoComplete(inst)
	local function Complete(instruction)
		local available = {} -- list of available commands and cvars
		for _, cvar in pairs(self.mInterpreter.cvarManager.CVars) do
			table.insert(available, cvar.name)
		end
		for _, ccom in pairs(self.mInterpreter.ccommandManager.CCommands) do
			table.insert(available, ccom.name)
		end
		
		return AutoComplete(instruction, available)
	end

	while inst:sub(1, 1) == " " do
		inst = inst:sub(2)
	end
	local spacepos = inst:find(" ")
	local a, b
	if spacepos then
		a, b = Complete(inst:sub(1, spacepos-1)) 
		a = a.. inst:sub(spacepos)
	else
		a, b = Complete(inst)
	end
	table.sort(b, function(a, b) return a:lower() < b:lower() end)
	return a, b
end

function Console:RenderTo(target)
	if not self.isOpened then
		return
	end
	target:Draw(self.mRect)
	if self.mOutputHasChanged then
		self:UpdateOutputText()
	end
	if self.mInputHasChanged then
		self:UpdateInputText()
	end
	target:Draw(self.mOutputText)
	target:Draw(self.mInputText)
end