require "TestVector.lua"
require "TestEuler.lua"
require "TestColor.lua"
require "TestEntity.lua"
require "TestTypeVerification.lua"

fenv ={}

-- globals
fenv._G = fenv
fenv.FLUSH = "[flush]"
fenv.INSERT = "[insert]"

fenv.self = TestEntity:New() --I always use this entity.

-- functions
fenv.print = function(...)
	for _, val in ipairs{...} do
		if type(val) == "table" then
			error("print called with table", 3) --although this may be possible
		end
	end
end
fenv.randomInt = function(min, max, ...)
	if #{...} > 0 then error("randomInt/Float called with too many arguments", 2) end
	if not tonumber(min) or not tonumber(max) then error("randomInt called with non-number argument", 2) end
	return tonumber(min)
end
fenv.randomFloat = fenv.randomInt
fenv.affect = function(name, method, func, ...)
	CheckNumParam({...}, "affect")
	CheckString(name, "name", "affect")
	CheckFunction(func, "func", "affect")
	if method ~= fenv.FLUSH and method ~= fenv.INSERT then error("affect: invalid method", 2) end
	func()
end
fenv.vector = function(x, y, z, ...)
	CheckNumParam({...}, "vector")
	return TestVector:New(x, y, z)
end
fenv.euler = function(pitch, yaw, roll, ...)
	CheckNumParam({...}, "euler")
	return TestEuler:New(pitch, yaw, roll)
end
fenv.task = function(name, func, ...)
	CheckNumParam({...}, "task")
	CheckString(name, "name", "task")
	CheckFunction(func, "func", "task")
	func()
end
fenv.run = function(name, ...)
	CheckNumParam({...}, "run")
	CheckString(name, "name", "run")
end
fenv.get = function(varname, vartype, ...)
	CheckNumParam({...}, "get")
	CheckString(varname, "varname", "get")
	if vartype == "string" then
		return ""
	elseif vartype == "float" then
		return 0
	elseif vartype == "vector" then
		return TestVector:New(0, 0, 0)
	else
		error("get called with invalid vartype", 2)
	end
end
fenv.set = function(varname, value, ...)
	CheckNumParam({...}, "set")
	CheckString(varname, "varname", "set")
end
fenv.kill = function(targetname, ...)
	CheckNumParam({...}, "kill")
	CheckString(targetname, "targetname", "kill")
end
fenv.remove = function(targetname, ...)
	CheckNumParam({...}, "remove")
	CheckString(targetname, "targetname", "remove")
end
fenv.use = function(targetname, ...)
	CheckNumParam({...}, "use")
	CheckString(targetname, "targetname", "use")
end
fenv.declare = function(varname, vartype, ...)
	CheckNumParam({...}, "declare")
	CheckString(varname, "varname", "declare")
	if vartype ~= "float" and vartype ~= "string" and vartype ~= "vector" then error("declare called with invalid vartype", 2) end
end
fenv.signal = function(signalname, ...)
	CheckNumParam({...}, "signal")
	CheckString(signalname, "signalname", "signal")
end
fenv.waitSignal = function(signalname, ...)
	CheckNumParam({...}, "waitsignal")
	CheckString(signalname, "signalname", "waitsignal")
end
fenv.wait = function(param, ...)
	CheckNumParam({...}, "wait")
	if type(param) ~= "number" and type(param) ~= "string" then error("wait called with invalid parameter", 2) end
end
fenv.camera = {}
fenv.camera.panTo = function(angle, dir, time, ...)
	CheckNumParam({...}, "camera.panTo")
	CheckEuler(angle, "angle", "camera.panTo")
	CheckVector(dir, "dir", "camera.panTo")
	CheckNumber(time, "time", "camera.panTo")
end
fenv.camera.zoomTo = function(fov, time, ...)
	CheckNumParam({...}, "camera.zoomTo")
	CheckNumber(fov, "fov", "camera.zoomTo")
	CheckNumber(time, "time", "camera.zoomTo")
end
fenv.camera.moveTo = function(pos, time, ...)
	CheckNumParam({...}, "camera.moveTo")
	CheckVector(pos, "pos", "camera.moveTo")
	CheckNumber(time, "time", "camera.moveTo")
end
fenv.camera.fadeFromTo = function(col1, col2, time, ...)
	CheckNumParam({...}, "camera.fadeFromTo")
	CheckColor(col1, "startColor", "camera.fadeFromTo")
	CheckColor(col2, "endColor", "camera.fadeFromTo")
	CheckNumber(time, "time", "camera.fadeFromTo")
end
fenv.camera.playRoff = function(filename, ...)
	CheckNumParam({...}, "camera.playRoff")
	CheckString(filename, "filename", "camera.playRoff")
end
fenv.camera.enable = function(...)
	CheckNumParam({...}, "camera.enable")
end
fenv.camera.disable = function(...)
	CheckNumParam({...}, "camera.disable")
end
fenv.camera.shakeFor = function(intensity, duration, ...)
	CheckNumParam({...}, "camera.shakeFor")
	CheckNumber(intensity, "intensity", "camera.shakeFor")
	CheckNumber(duration, "duration", "camera.shakeFor")
end
fenv.camera.roll = function(angle, time, ...)
	CheckNumParam({...}, "camera.roll")
	CheckNumber(angle, "angle", "camera.roll")
	CheckNumber(time, "time", "camera.roll")
end
fenv.camera.track = function(name, speed, lerp, ...)
	CheckNumParam({...}, "camera.track")
	CheckString(name, "name", "camera.track")
	CheckNumber(speed, "speed", "camera.track")
	CheckBool(lerp, "lerp", "camera.track")
end
fenv.camera.distance = function(distance, lerp, ...)
	CheckNumParam({...}, "camera.distance")
	CheckNumber(distance, "distance", "camera.distance")
	CheckBool(lerp, "lerp", "camera.distance")
end
fenv.camera.follow = function(groupname, speed, lerp, ...)
	CheckNumParam({...}, "camera.follow")
	CheckString(groupname, "groupname", "camera.follow")
	CheckNumber(speed, "speed", "camera.follow")
	CheckBool(lerp, "lerp", "camera.follow")
end
fenv.getEntity = function(name, ...)
	CheckNumParam({...}, "getEntity")
	CheckString(name, "name", "getEntity")
	return fenv.self
end
fenv.doTask = function(name, ...)
	CheckNumParam({...}, "doTask")
	CheckString(name, "name", "doTask")
end
fenv.flush = function(...)
	CheckNumParam({...}, "flush")
end
fenv.vectorAndAlphaToColor = vectorAndAlphaToColor
fenv.floor = math.floor