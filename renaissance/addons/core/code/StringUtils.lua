function explode(str, sep)
  local t, ll
  t={}
  ll=0
  if(#str == 1) then return {str} end
    while true do
      l=string.find(str,sep,ll,true) -- find the next d in the string
      if l~=nil then -- if "not not" found then..
        table.insert(t, string.sub(str,ll,l-1)) -- Save it in our array.
        ll=l+1 -- save just after where we found it for searching next time.
      else
        table.insert(t, string.sub(str,ll)) -- Save what's left in our array.
        break -- Break at end, as it should be, according to the lua manual.
      end
    end
  return t
end

function Escape(str)
	local output = ""
	for i = 1, #str do
		local char = str:sub(i, i)
		--escape any escape signs and quotes
		if char == "\\" or char == "\"" then
			char = "\\" .. char
		--and newlines
		elseif char == "\n" then
			char = "\\n"
		--is this even possible?
		elseif char == "\0" then
			char = "\\0"
		end
		output = output .. char
	end
	return output
end