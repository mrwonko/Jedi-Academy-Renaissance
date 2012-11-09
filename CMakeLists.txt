cmake_minimum_required(VERSION 2.6)

message("\n== Jedi Academy: Renaissance ==\n")

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
	message("Target: Debug")
else(${CMAKE_BUILD_TYPE} MATCHES Debug)
	message("Target: Release")
endif(${CMAKE_BUILD_TYPE} MATCHES Debug)

# add cmake_modules to module search path, for find_package()
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
# I force CMAKE_ROOT/Modules into the module search path because it's missing in my cmake at uni - maybe that's a local problem though?
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_ROOT}/Modules")

function(find_zlib)
set(ZLIB_HINTS
		${ZLIB_ROOT}
		$ENV{ZLIB_ROOT})
set(ZLIB_SEARCH_DIRS
		/usr/local/
		/usr/)
	find_path(ZLIB_INCLUDE_DIR zlib.h
		PATH_SUFFIXES "include"
		HINTS ${ZLIB_HINTS}
		PATHS ${ZLIB_SEARCH_DIRS})
	find_library(ZLIB_LIBRARY zdll zlib zlib1 z
		PATH_SUFFIXES "lib"
		HINTS ${ZLIB_HINTS}
		PATHS ${ZLIB_SEARCH_DIRS})
	if((NOT ZLIB_INCLUDE_DIR) OR (NOT ZLIB_LIBRARY))
		message(FATAL_ERROR "Could not find ZLIB, try setting ZLIB_ROOT")
	endif((NOT ZLIB_INCLUDE_DIR) OR (NOT ZLIB_LIBRARY))
	set(ZLIB_INCLUDE_DIR ${ZLIB_INCLUDE_DIR} PARENT_SCOPE)
	set(ZLIB_LIBRARY ${ZLIB_LIBRARY} PARENT_SCOPE)
endfunction(find_zlib)

project(JediAcademyRenaissance)

set(JAR_BINARY_DIR "${JediAcademyRenaissance_SOURCE_DIR}/renaissance/bin")

# add local include directories
include_directories("dependencies/minizip-jar" "dependencies/lua-jar/include" "dependencies/luabind-jar" "dependencies/pluto-jar" "code/jarlib/include")

# add subdirectories - parameters: source dir, binary dir
add_subdirectory("dependencies/minizip-jar")
add_subdirectory("dependencies/lua-jar")
add_subdirectory("dependencies/luabind-jar")
add_subdirectory("dependencies/pluto-jar")
add_subdirectory("code/jarlib")
add_subdirectory("code/renaissance")
