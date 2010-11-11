function StringVectorIterator(stringvector)
	local i = 0
	return function()
		if i < #stringvector then
			local val = stringvector:at(i)
			i = i + 1
			return val
		end
	end
end