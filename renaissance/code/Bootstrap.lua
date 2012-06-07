--  Replace Lua functions so they work as expected

-- replace file functions

--loadfile, used by dofile
function loadfile(filename)
	if type(filename) ~= "string" then
		error("loadfile() called with non-string argument!", 2)
	end
	
	local file = jar.fs.OpenRead(filename)
	
	if file == 0 or file == nil or not file then
		return nil, "Could not open " .. filename .. "! " .. jar.fs.GetLastError()
	end
	local success, content = file:GetContent()
	file:Close()
	if not success then
		return nil, "Could not read " .. filename .. "! " .. jar.fs.GetLastError()
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
	if type(filename) ~= "string" then
		error("require() called with non-string argument!", 2)
	end
	
	local file = jar.fs.OpenRead(filename)
	local name = filename
	if not file then
		name = filename .. ".lua"
		file = jar.fs.OpenRead(name)
	end
	if not file then
		name = "code/" .. filename
		file = jar.fs.OpenRead(name)
	end
	if not file then
		name = "code/" .. filename .. ".lua"
		file = jar.fs.OpenRead(name)
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
package.loaders = { mrwloadmodule }

--TODO: replace with assets1.pk3 loading, put this there
if not RELEASE then
assert(jar.fs.Mount("./", true)) -- append, i.e. last resort
else
assert(jar.fs.Mount("assets1.pk3", false)) --prepend, i.e. search first
end

-- error callback function for xpcall()
function AddTracebackToError(err)
	if type(err) == "string" then
		return debug.traceback(err, 3)
	else
		return err
	end
end

local success, err = xpcall(loadfile("code/Init.lua"), AddTracebackToError)
if not success then
	jar.Logger.GetDefaultLogger():Error(err)
end