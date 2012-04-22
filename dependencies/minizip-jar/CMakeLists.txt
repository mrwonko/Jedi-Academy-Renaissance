cmake_minimum_required(VERSION 2.6)

message("\n= Minizip =\n")

if(NOT JAR_BINARY_DIR)
	message(FATAL_ERROR "Use the toplevel cmake script!")
endif(NOT JAR_BINARY_DIR)

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
	add_definitions(-Dlua_assert=assert)
	set(JAR_MINIZIP_NAME "jar_minizip_d")
else(${CMAKE_BUILD_TYPE} MATCHES Debug)
	set(JAR_MINIZIP_NAME "jar_minizip")
endif(${CMAKE_BUILD_TYPE} MATCHES Debug)
# name visible to parent scope as well
set(JAR_MINIZIP_NAME ${JAR_MINIZIP_NAME} PARENT_SCOPE) # note the PARENT_SCOPE

find_zlib() # CMake's FindZLIB module kinda sucks

add_library(${JAR_MINIZIP_NAME} SHARED "ioapi.c" "unzip.c" "ioapi.h" "unzip.h")

add_definitions(-DBUILD_DLL)
if(WIN32)
	add_definitions(-DLUA_BUILD_AS_DLL)
endif(WIN32)

include_directories(${ZLIB_INCLUDE_DIR})
target_link_libraries(${JAR_MINIZIP_NAME} ${ZLIB_LIBRARY})

install(TARGETS ${JAR_MINIZIP_NAME}
	ARCHIVE DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/lib"
	LIBRARY DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/lib"
	RUNTIME DESTINATION "${JAR_BINARY_DIR}")

message("${JAR_MINIZIP_NAME} target generated.")