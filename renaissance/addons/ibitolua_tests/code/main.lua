--function environment for the scripts

require "TestFEnv.lua"
require "ibi_to_lua.lua"

local rejoice = false
local verbose = false

local function RunCode(code, filename)
	if verbose then
		jar.Logger.GetDefaultLogger():Log("Running " .. filename)
	end
	-- prevent infinite loops
	code = code:gsub("while", "do --")
	local codeFunc, errorMessage = loadstring(code, "@" .. filename)
	if not codeFunc then
		jar.Logger.GetDefaultLogger():Warning("Error compiling " .. filename .. ":\n" .. errorMessage)
		return false
	end
	setfenv(codeFunc, fenv)
	local success, errorMessage = pcall(codeFunc)
	if not success then
		jar.Logger.GetDefaultLogger():Warning("Error running " .. filename .. ":\n" .. errorMessage)
		return false
	end
	if rejoice then
		jar.Logger.GetDefaultLogger():Log("Passed " .. filename)
	end
	return true
end

if true then
--if false then
	local scriptpath = "scripts/tests/"
	local total = 0
	local successful = 0
	for _, dirname in ipairs(jar.fs.GetDirectoriesInDirectory(scriptpath)) do
		for _, filename in ipairs(jar.fs.GetFilesInDirectory(scriptpath .. dirname)) do
			if filename:sub(-4):lower() == ".ibi" then
				local fullname = scriptpath .. dirname .. "/" .. filename
				local code, errorMessage = IbiToLua(fullname)
				total = total + 1
				if not code then
					jar.Logger.GetDefaultLogger():Warning("Error converting " .. fullname .. ":\n" .. errorMessage)
				else
					if RunCode(code, fullname) then
						successful = successful + 1
					else
						jar.Logger.GetDefaultLogger():Log("\n" .. code .. "\n")
					end
				end
			elseif verbose then
				jar.Logger.GetDefaultLogger():Log("No .ibi: " .. scriptpath .. dirname .. "/" .. filename)
			end
		end
	end

	jar.Logger.GetDefaultLogger():Log(successful .. " out of " .. total .. " files compiled properly.")
end