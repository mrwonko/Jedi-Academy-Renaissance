--  Replace Lua functions so they work as expected

-- replace file functions

--loadfile, used by dofile
function loadfile(filename)
	local file = jar.fs.OpenRead(filename)
	if not file then
		return nil, "Could not open " .. filename .. "!"
	end
	local success, content = file:GetContent()
	file:Close()
	if not success then
		return nil, "Could not read " .. filename .. "!"
	end
	return loadstring(content, "@" .. filename)
end

function dofile(filename)
	local f, err = loadfile(filename)
	if not f then
		error(err, 2)
	end
	f()
end

--module loader for require
local function mrwloadmodule(filename)
	local file = jar.fs.OpenRead(filename)
	local name = filename
	if not file then
		file = jar.fs.OpenRead(filename .. ".lua")
		name = filename .. ".lua"
	end
	if not file then
		file = jar.fs.OpenRead("code/" .. filename)
		name = "code/" .. filename
	end
	if not file then
		file = jar.fs.OpenRead("code/" .. filename .. ".lua")
		name = "code/" .. filename .. ".lua"
	end
	if not file then
		return "\n\tCould not find " .. filename .." in pk3s!"
	end
	local success, content = file:GetContent()
	if not success then
		file:Close()
		return "\n\tCould not read " .. name .. "!"
	end
	file:Close()
	local f, err = loadstring(content, "@" .. name)
	if f then
		return f
	end
	error(err, 3)
end
--I only want my loader, so... let's just replace all the others.
package.loaders = { [1] = mrwloadmodule }

--TODO: replace with assets1.pk3 loading, put this there
jar.fs.Mount("./", true) -- append, i.e. last resort (TODO: DOCUMENT!)
--jar.fs.Mount("assets1.pk3", false) --prepend, i.e. search first
dofile("code/Init.lua")