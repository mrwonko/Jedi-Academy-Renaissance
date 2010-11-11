--  Replace Lua functions so they work as expected

-- replace file functions

--low level load file function, used by loadfile and dofile and require
local function lowLoadFile(filename)
	local file = physFS.OpenRead(filename)
	if not file then
		return nil, "Could not open " .. filename .. "!"
	end
	local success, content = file:GetContent(content)
	if not success then
		file:Close()
		return nil, "Could not read " .. filename .. "!"
	end
	file:Close()
	return loadstring(content, "@" .. filename)
end
loadfile = lowLoadFile

function dofile(filename)
	local f, err = lowLoadFile(filename)
	if not f then
		error(err, 2)
	end
	f()
end

--module loader for require
local function mrwloadmodule(filename)
	local file = physFS.OpenRead(filename)
	local name = filename
	if not file then
		file = physFS.OpenRead(filename .. ".lua")
		name = filename .. ".lua"
	end
	if not file then
		file = physFS.OpenRead("code/" .. filename)
		name = "code/" .. filename
	end
	if not file then
		file = physFS.OpenRead("code/" .. filename .. ".lua")
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
physFS.Mount("./", true) -- append, i.e. last resort (TODO: DOCUMENT!)
--physFS.Mount("assets1.pk3", false) --prepend, i.e. search first
require("code/Init.lua")