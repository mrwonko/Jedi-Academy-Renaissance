cmake_minimum_required(VERSION 2.6)

message("\n= Pluto =\n")

if(NOT JAR_BINARY_DIR)
	message(FATAL_ERROR "Use the toplevel cmake script!")
endif(NOT JAR_BINARY_DIR)

if(NOT JAR_LUA_NAME)
	message(FATAL_ERROR "Internal logic error - Lua not built before pluto!")
endif(NOT JAR_LUA_NAME)

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
	add_definitions(-Dlua_assert=assert -DPLUTO_DEBUG)
	set(JAR_PLUTO_NAME "jar_pluto_d")
else(${CMAKE_BUILD_TYPE} MATCHES Debug)
	set(JAR_PLUTO_NAME "jar_pluto")
endif(${CMAKE_BUILD_TYPE} MATCHES Debug)
# make name visible to parent scope as well
set(JAR_PLUTO_NAME ${JAR_PLUTO_NAME} PARENT_SCOPE)

add_definitions(-DPLUTO_BUILD_DLL)
if(WIN32)
	add_definitions(-DLUA_BUILD_AS_DLL)
endif(WIN32)

add_library(${JAR_PLUTO_NAME} SHARED "lzio.c" "pdep.c" "pluto.c" "pluto.h")
target_link_libraries(${JAR_PLUTO_NAME} ${JAR_LUA_NAME})

if(WIN32)
	install(TARGETS ${JAR_PLUTO_NAME}
		ARCHIVE DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/lib"
		RUNTIME DESTINATION "${JAR_BINARY_DIR}")
else(WIN32)
	install(TARGETS ${JAR_PLUTO_NAME}
		LIBRARY DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/lib")
	install(TARGETS ${JAR_PLUTO_NAME}
		LIBRARY DESTINATION "${JAR_BINARY_DIR}")
endif(WIN32)

message("${JAR_PLUTO_NAME} target generated.")
