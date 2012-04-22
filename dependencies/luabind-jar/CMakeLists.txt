cmake_minimum_required(VERSION 2.6)

message("\n= Luabind =\n")

if(NOT JAR_BINARY_DIR)
	message(FATAL_ERROR "Use the toplevel cmake script!")
endif(NOT JAR_BINARY_DIR)

if(NOT JAR_LUA_NAME)
	message(FATAL_ERROR "Internal logic error - Lua not built before luabind!")
endif(NOT JAR_LUA_NAME)

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
	#add_definitions(-Dlua_assert=assert)
	set(JAR_LUABIND_NAME "jar_luabind_d")
else(${CMAKE_BUILD_TYPE} MATCHES Debug)
	set(JAR_LUABIND_NAME "jar_luabind")
endif(${CMAKE_BUILD_TYPE} MATCHES Debug)
# make name visible to parent scope as well
set(JAR_LUABIND_NAME ${JAR_LUABIND_NAME} PARENT_SCOPE)

if(NOT Boost_FOUND)
	find_package(Boost REQUIRED)
endif()
include_directories(${Boost_INCLUDE_DIRS})

add_definitions(-DLUABIND_DYNAMIC_LINK)
if(WIN32)
	add_definitions(-DLUA_BUILD_AS_DLL)
endif(WIN32)

add_subdirectory(src)

message("${JAR_LUABIND_NAME} target generated.")