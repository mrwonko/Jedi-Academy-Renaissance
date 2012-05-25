-- Test expressions

require("Expression.lua")

local two = Expression:New(Expression.Type.Constant, 2)
local test = 4 + two * 5
print("4 + 2 * 5 = " .. test:Evaluate())

local var = Expression:New(Expression.Type.Variable, "var")
local test2 = 2 * 4 + var * 5
for _, i in ipairs{1, 2, 5} do
	vars = {var = i}
	print("4 * 2 + " .. i .. " * 5 = " .. test2:ToString(vars) .. " = " .. test2:Evaluate(vars))
end


-- Test layouts
require("LayoutManager.lua")

g_layoutManager = LayoutManager:New()
for filename in jar.fs.GetFilesInDirectory("code/ui/layouts"):items() do
	g_layoutManager:ParseFile("code/ui/layouts/" .. filename)
end