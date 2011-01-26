-- coroutine tests

function retTest()
	return 1, 2, 3
end

--variable number of return values: use as constructor of table
local ret = {retTest()}
for i, v in ipairs(ret) do
	print(v)
end


print("==== Coroutine Tests ====")

local instructions =
{
	"test()",
	"wait 5",
	"doStuff()",
	"a = 5",
	"wait 1",
	"me = \"awesome\""
}

function coroutineFunction(instructions)
	for i, instruction in ipairs(instructions) do
		if instruction:match("wait %d+") == instruction then
			coroutine.yield(tonumber(instruction:sub(6)))
		else
			print(instruction)
		end
	end
end

local co = coroutine.create(coroutineFunction)
while true do
	local ret = {coroutine.resume(co, instructions)}
	if not ret[1] then
		print(ret[2])
		break
	end
	if coroutine.status(co) == "suspended" then
		print("waiting " .. ret[2] .. " seconds...")
	end
end

print("==== Coroutine Tests over ====")