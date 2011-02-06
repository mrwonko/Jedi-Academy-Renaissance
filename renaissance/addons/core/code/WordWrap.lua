--[[!	\brief Creates new lines to make a text fit a given maximum width.
		\param fontsize optional, if it's nil the fonts default size will be used.
		\return Array of lines
--]]

function WordWrap(str, font, fontsize, availableWidth)
	if not str then error("WordWrap(): No string supplied!", 2) end
	if not font then error("WordWrap(): No font supplied!", 2) end
	if not fontsize then fontsize = font:GetDefaultSize() end
	if not availableWidth then error("WordWrap(): No availableWidth supplied!", 2) end
	local factor = fontsize / font:GetDefaultSize()
	
	local lines = {}
	local curline = ""
	--this will be used in case there's enough space to add another word
	local spacer = ""
	
	while str ~= "" do
		local nextSpacer = ""
		--find first space/tab
		local curWord = ""
		local pos = str:find(" ")
		local tabPos = str:find("\t")
		local newlinePos = str:find("\n")
		if pos then
			nextSpacer = " "
		end
		if tabPos and (not pos or tabPos < pos) then
			pos = tabPos
			nextSpacer = "\t"
		end
		if newlinePos and (not pos or newlinePos < pos) then
			pos = newlinePos
			nextSpacer = "\n"
		end
		if pos then
			curWord = str:sub(1, pos-1)
			str = str:sub(pos+1)
		else
			curWord = str
			str = ""
		end
		
		--newline?
		if spacer == "\n" then
			table.insert(lines, curline)
			curline = ""
			spacer = ""
		end
		
		--check length
		if factor * font:GetWidth(curline .. spacer .. curWord) <= availableWidth then
			--enough space
			curline = curline .. spacer .. curWord
		else
			--not enough space
			--is there not enough space on the next line?
			if factor * font:GetWidth(curWord) > availableWidth then
				--no, there isn't. add as much as possible to the current line.
				for len = 1, #curWord do
					--would this much not fit anymore?
					if factor * font:GetWidth(curline .. spacer .. curWord:sub(1, len)) > availableWidth then
						-- make sure the line is not completely full
						if len ~= 1 then
							curline = curline .. spacer .. curWord:sub(1, len-1)
							curWord = curWord:sub(len)
						elseif #curline == 0 then --first letter and already too much? the line better not be empty then, or we'd have an infinite loop
							error("WordWrap(): Not enough width for a single letter!", 2)
						end
						break
					end
					--if this was the case, the word would actually fit on the current line and this whole block should never have been entered!
					assert(len ~= #curWord)
				end
			end
			str = curWord .. nextSpacer .. str
			nextSpacer = ""
			table.insert(lines, curline)
			curline = ""
		end
		spacer = nextSpacer
	end
	
	table.insert(lines, curline)
	
	return lines
end