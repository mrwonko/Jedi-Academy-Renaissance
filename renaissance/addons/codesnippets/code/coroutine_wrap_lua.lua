-- the original coroutine.wrap returns a C function, apparently - which is not persistable.
-- this is a partial re-implementation in Lua
-- TODO: add error handling to my implementation
do
	local remove = table.remove
	local resume = coroutine.resume
	local create = coroutine.create
	local unpack = unpack
	
	coroutine.wrap = function(f)
		print("Wrap!")
		local co = create(f)
		return function(...)
			local ret = {resume(co, ...)}
			remove(ret, 1)
			return unpack(ret)
		end
	end
end