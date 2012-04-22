cmake_minimum_required(VERSION 2.6)

message("\n= Lua =\n")

if(NOT JAR_BINARY_DIR)
	message(FATAL_ERROR "Use the toplevel cmake script!")
endif(NOT JAR_BINARY_DIR)

set(LUA_HEADER_FILES "src/lapi.c" "src/lauxlib.c" "src/lbaselib.c" "src/lcode.c" "src/ldblib.c" "src/ldebug.c" "src/ldo.c" "src/ldump.c" "src/lfunc.c" "src/lgc.c" "src/linit.c" "src/liolib.c" "src/llex.c" "src/lmathlib.c" "src/lmem.c" "src/loadlib.c" "src/lobject.c" "src/lopcodes.c" "src/loslib.c" "src/lparser.c" "src/lstate.c" "src/lstring.c" "src/lstrlib.c" "src/lstate.c" "src/lstring.c" "src/lstrlib.c" "src/ltable.c" "src/ltablib.c" "src/ltm.c" "src/lundump.c" "src/lvm.c" "src/lzio.c")
set(LUA_SOURCE_FILES "src/lapi.h" "src/lauxlib.h" "src/lcode.h" "src/ldebug.h" "src/ldo.h" "src/lfunc.h" "src/lgc.h" "src/llex.h" "src/llimits.h" "src/lmem.h" "src/lobject.h" "src/lopcodes.h" "src/lparser.h" "src/lstate.h" "src/lstring.h" "src/ltable.h" "src/ltm.h" "src/lua.h" "src/luaconf.h" "src/lualib.h" "src/lundump.h" "src/lvm.h" "src/lzio.h")


if(${CMAKE_BUILD_TYPE} MATCHES Debug)
	add_definitions(-Dlua_assert=assert)
	set(JAR_LUA_NAME "jar_lua_d")
else(${CMAKE_BUILD_TYPE} MATCHES Debug)
	set(JAR_LUA_NAME "jar_lua")
endif(${CMAKE_BUILD_TYPE} MATCHES Debug)
# make name visible to parent scope as well
set(JAR_LUA_NAME ${JAR_LUA_NAME} PARENT_SCOPE) # note the PARENT_SCOPE

add_library(${JAR_LUA_NAME} SHARED ${LUA_SOURCE_FILES} ${LUA_HEADER_FILES})

if(WIN32)
	add_definitions(-DLUA_BUILD_AS_DLL)
endif(WIN32)

if(CMAKE_COMPILER_IS_GNUCXX)
	target_link_libraries(${JAR_LUA_NAME} "m")
endif()

install(TARGETS ${JAR_LUA_NAME}
	ARCHIVE DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/lib"
	LIBRARY DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/lib"
	RUNTIME DESTINATION "${JAR_BINARY_DIR}")

message("${JAR_LUA_NAME} target generated.")