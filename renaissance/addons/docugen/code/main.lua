print("Todo: generate documentation, duh")

print() --newline

for name, test in pairs{print=print, num=2, event=jar.Event, unzOpen=unz.Open} do
	info = jar.GetLuabindInfo(test)
	if not info then
		print(name .. " is no Luabind thingy")
	else
		if info.type == "function" then
			print((info.name or name) .. " is a luabind function. Signature(s):")
			for _, signature in ipairs(info.overloads) do
				print("  " .. signature)
			end
		else
			assert(info.type == "class")
			print((info.name or name) .. " is a luabind class")
		end
	end
	print() --newline
end