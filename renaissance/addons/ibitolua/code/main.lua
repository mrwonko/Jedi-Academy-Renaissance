---
-- User: Willi
-- Date: 04.12.11
-- Time: 12:21
--

require "ibi_to_lua.lua"

local filename = "scripts/roof.ibi"
print("Converting \""..filename.."\" to Lua...")
print(IbiToLua(filename))