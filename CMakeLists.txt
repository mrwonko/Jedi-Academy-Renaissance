cmake_minimum_required(VERSION 2.6)

message("\n== Jedi Academy: Renaissance ==\n")

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
	message("Target: Debug")
else(${CMAKE_BUILD_TYPE} MATCHES Debug)
	message("Target: Release")
endif(${CMAKE_BUILD_TYPE} MATCHES Debug)

# add cmake_modules to module search path, for find_package()
# I force CMAKE_ROOT/Modules into the module search path because it's missing in my cmake at uni - maybe that's a local problem though?
# yeah, let's not do that. it causes warnings.
#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH} "${CMAKE_ROOT}/Modules")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

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

#   add local include directories
include_directories("dependencies/minizip-jar" "dependencies/lua-jar/include" "dependencies/luabind-jar" "dependencies/pluto-jar" "code/jarlib/include")

#   add subdirectories
add_subdirectory("dependencies/minizip-jar")
add_subdirectory("dependencies/lua-jar")
add_subdirectory("dependencies/luabind-jar")
add_subdirectory("dependencies/pluto-jar")

#   add packages used by both jarlib and renaissance.exe

# find Bullet Physics
if(NOT BULLET_ROOT)
	set(BULLET_ROOT "$ENV{BULLET_ROOT}")
endif(NOT BULLET_ROOT)
find_package(Bullet)
if(NOT BULLET_FOUND)
	message(FATAL_ERROR "Could not find Bullet Physics - install it and/or set BULLET_ROOT accordingly.")
endif(NOT BULLET_FOUND)
if((NOT BULLET_DYNAMICS_LIBRARY_DEBUG) OR (NOT BULLET_MATH_LIBRARY_DEBUG) OR (NOT BULLET_COLLISION_LIBRARY_DEBUG))
	message(FATAL_ERROR "Bullet Physics Debug libraries not found!")
endif((NOT BULLET_DYNAMICS_LIBRARY_DEBUG) OR (NOT BULLET_MATH_LIBRARY_DEBUG) OR (NOT BULLET_COLLISION_LIBRARY_DEBUG))

if(MSVC)
	set(BULLET_DYNAMICS_LIBRARY optimized ${BULLET_DYNAMICS_LIBRARY} debug ${BULLET_DYNAMICS_LIBRARY_DEBUG})
	set(BULLET_COLLISION_LIBRARY optimized ${BULLET_COLLISION_LIBRARY} debug ${BULLET_COLLISION_LIBRARY_DEBUG})
	set(BULLET_MATH_LIBRARY optimized ${BULLET_MATH_LIBRARY} debug ${BULLET_MATH_LIBRARY_DEBUG})
else(MSVC)
	if(${CMAKE_BUILD_TYPE} MATCHES Debug)
		set(BULLET_DYNAMICS_LIBRARY ${BULLET_DYNAMICS_LIBRARY_DEBUG})
		set(BULLET_COLLISION_LIBRARY ${BULLET_COLLISION_LIBRARY_DEBUG})
		set(BULLET_MATH_LIBRARY ${BULLET_MATH_LIBRARY_DEBUG})
	endif(${CMAKE_BUILD_TYPE} MATCHES Debug)
endif(MSVC)

include_directories("${BULLET_INCLUDE_DIRS}")

# find OpenGL
find_package(OpenGL REQUIRED)
if(OPENGL_INCLUDE_DIR) #may be empty
	include_directories("${OPENGL_INCLUDE_DIR}")
endif(OPENGL_INCLUDE_DIR)

# find GLEW
if(NOT GLEW_ROOT_DIR)
	set(GLEW_ROOT_DIR "$ENV{GLEW_ROOT}")
endif(NOT GLEW_ROOT_DIR)
find_package(GLEW)
if(NOT GLEW_FOUND)
	message(FATAL_ERROR "Could not find GLEW - install it and/or set GLEW_ROOT accordingly")
endif(NOT GLEW_FOUND)
include_directories("${GLEW_INCLUDE_PATH}")

add_subdirectory("code/jarlib")
add_subdirectory("code/renaissance")
