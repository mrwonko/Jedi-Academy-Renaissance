cmake_minimum_required(VERSION 2.6)

message("\n= JARLib =\n")

if(NOT JAR_BINARY_DIR)
	message(FATAL_ERROR "Use the toplevel cmake script!")
endif(NOT JAR_BINARY_DIR)

#   dependencies, settings

# on windows, #define LUA_BUILD_AS_DLL
if(WIN32)
	add_definitions(-DLUA_BUILD_AS_DLL)
endif(WIN32)

# #define LUABIND_DYNAMIC_LINK
add_definitions(-DLUABIND_DYNAMIC_LINK)

# find BOOST and add it to include directories (so Luabind works)
if(NOT Boost_FOUND)
	find_package(Boost REQUIRED)
endif()
include_directories("${Boost_INCLUDE_DIRS}")

# find SFML and add it to include directories
find_package(SFML 2 REQUIRED system window graphics audio)
include_directories("${SFML_INCLUDE_DIR}")
add_definitions(-DSFML_DYNAMIC) # this may be outdated? there's SFML_STATIC, this may be assumed otherwise...

# find ZLIB
find_zlib()
include_directories("${ZLIB_INCLUDE_DIR}")

# find PhysicsFS
find_package(PhysFS REQUIRED)
include_directories("${PHYSFS_INCLUDE_DIR}")

# Debug/Release differences
if(${CMAKE_BUILD_TYPE} MATCHES Debug)
	add_definitions(-D_DEBUG)
	set(JARLIB_CORE_NAME "jarlib_core_d")
	set(JARLIB_INPUT_NAME "jarlib_input_d")
	set(JARLIB_GRAPHICS_NAME "jarlib_graphics_d")
	set(JARLIB_AUDIO_NAME "jarlib_audio_d")
	set(SFML_GRAPHICS "${SFML_GRAPHICS_LIBRARY_DEBUG}")
	set(SFML_AUDIO "${SFML_AUDIO_LIBRARY_DEBUG}")
	set(SFML_WINDOW "${SFML_WINDOW_LIBRARY_DEBUG}")
	set(SFML_SYSTEM "${SFML_SYSTEM_LIBRARY_DEBUG}")
else(${CMAKE_BUILD_TYPE} MATCHES Debug)
	set(JARLIB_CORE_NAME "jarlib_core")
	set(JARLIB_INPUT_NAME "jarlib_input")
	set(JARLIB_GRAPHICS_NAME "jarlib_graphics")
	set(JARLIB_AUDIO_NAME "jarlib_audio")
	set(SFML_GRAPHICS "${SFML_GRAPHICS_LIBRARY_RELEASE}")
	set(SFML_AUDIO "${SFML_AUDIO_LIBRARY_RELEASE}")
	set(SFML_WINDOW "${SFML_WINDOW_LIBRARY_RELEASE}")
	set(SFML_SYSTEM "${SFML_SYSTEM_LIBRARY_RELEASE}")
endif(${CMAKE_BUILD_TYPE} MATCHES Debug)
# make jarlib names visible in parent space
set(JARLIB_CORE_NAME ${JARLIB_CORE_NAME} PARENT_SCOPE)
set(JARLIB_INPUT_NAME ${JARLIB_INPUT_NAME} PARENT_SCOPE)
set(JARLIB_GRAPHICS_NAME ${JARLIB_GRAPHICS_NAME} PARENT_SCOPE)
set(JARLIB_AUDIO_NAME ${JARLIB_AUDIO_NAME} PARENT_SCOPE)

# the files of the different libraries

set(JARLIB_CORE_FILES
	"src/jar/core/CLArguments.cpp"
	"src/jar/core/Component.cpp"
	"src/jar/core/CoreImpl.cpp"
	"src/jar/core/CoutAndFileLogger.cpp"
	"src/jar/core/CoutLogger.cpp"
	"src/jar/core/FileLogger.cpp"
	"src/jar/core/FileSystem.cpp"
	"src/jar/core/GetLuabindInfo.cpp"
	"src/jar/core/Helpers.cpp"
	"src/jar/core/Logger.cpp"
	"src/jar/core/Lua.cpp"
	"src/jar/core/LuaLogger.cpp"
	"src/jar/core/Time.cpp"
	"src/jar/core/Updatable.cpp"
	"src/jar/luabind/LBCore.cpp"
	"src/jar/Core.cpp"
	
	"include/jar/core/API.hpp"
	"include/jar/core/CLArguments.hpp"
	"include/jar/core/Component.hpp"
	"include/jar/core/CoreImpl.hpp"
	"include/jar/core/CoutAndFileLogger.hpp"
	"include/jar/core/CoutLogger.hpp"
	"include/jar/core/FileLogger.hpp"
	"include/jar/core/FileSystem.hpp"
	"include/jar/core/GetLuabindInfo.hpp"
	"include/jar/core/Helpers.hpp"
	"include/jar/core/Logger.hpp"
	"include/jar/core/Lua.hpp"
	"include/jar/core/LuaLogger.hpp"
	"include/jar/core/Time.hpp"
	"include/jar/core/Updatable.hpp"
	"include/jar/core/VoidLogger.hpp"
	"include/jar/luabind/Core.hpp"
	"include/jar/Core.hpp"
	"include/jar/Jar.hpp"
	"include/jar/OS.hpp"
	)
set(JARLIB_INPUT_FILES
	"src/jar/input/luabind/LBEvent.cpp"
	"src/jar/input/luabind/LBInputDeviceController.cpp"
	"src/jar/input/luabind/LBInputDeviceManager.cpp"
	"src/jar/input/luabind/LBInputImpl.cpp"
	"src/jar/input/Event.cpp"
	"src/jar/input/EventManager.cpp"
	"src/jar/input/EventQueue.cpp"
	"src/jar/input/EventRelay.cpp"
	"src/jar/input/EventSender.cpp"
	"src/jar/input/InputDevice.cpp"
	"src/jar/input/InputDeviceController.cpp"
	"src/jar/input/InputDeviceManager.cpp"
	"src/jar/input/InputImpl.cpp"
	"src/jar/Input.cpp"
	
	"include/jar/input/API.hpp"
	"include/jar/input/Event.hpp"
	"include/jar/input/EventListener.hpp"
	"include/jar/input/EventManager.hpp"
	"include/jar/input/EventQueue.hpp"
	"include/jar/input/EventRelay.hpp"
	"include/jar/input/EventSender.hpp"
	"include/jar/input/InputDevice.hpp"
	"include/jar/input/InputDeviceController.hpp"
	"include/jar/input/InputDeviceManager.hpp"
	"include/jar/input/InputImpl.hpp"
	"include/jar/input/KeyCodes.hpp"
	"include/jar/Input.hpp"
	"include/jar/OS.hpp"
	)
set(JARLIB_GRAPHICS_FILES
	"src/jar/graphics/luabind/LBFont.cpp"
	"src/jar/graphics/luabind/LBRenderWindow.cpp"
	"src/jar/graphics/luabind/LBText.cpp"
	"src/jar/graphics/Font.cpp"
	"src/jar/graphics/GraphicsImpl.cpp"
	"src/jar/graphics/RenderWindow.cpp"
	"src/jar/graphics/Text.cpp"
	"src/jar/luabind/LBSFMLGraphics.cpp"
	"src/jar/Graphics.cpp"
	
	"include/jar/graphics/API.hpp"
	"include/jar/graphics/Font.hpp"
	"include/jar/graphics/GraphicsImpl.hpp"
	"include/jar/graphics/RenderWindow.hpp"
	"include/jar/graphics/Text.hpp"
	"include/jar/luabind/SFMLGraphics.hpp"
	"include/jar/Graphics.hpp"
	"include/jar/OS.hpp"
	)
set(JARLIB_AUDIO_FILES
	"src/jar/audio/luabind/LBSFMLAudio.cpp"
	"src/jar/audio/AudioImpl.cpp"
	"src/jar/Audio.cpp"
	
	"include/jar/audio/luabind/SFMLAudio.hpp"
	"include/jar/audio/API.hpp"
	"include/jar/audio/AudioImpl.hpp"
	"include/jar/OS.hpp"
	"include/jar/Audio.hpp"
	)

# libraries
set(JARLIB_CORE_LIBRARIES
	${ZLIB_LIBRARY}
	${JAR_LUA_NAME} ${JAR_LUABIND_NAME} ${JAR_PLUTO_NAME} ${JAR_MINIZIP_NAME}
	${SFML_SYSTEM}
	${PHYSFS_LIBRARY}
	)
set(JARLIB_INPUT_LIBRARIES
	${SFML_WINDOW} ${SFML_SYSTEM}
	${JARLIB_CORE_NAME}
	${JAR_LUA_NAME} ${JAR_LUABIND_NAME}
	)
set(JARLIB_GRAPHICS_LIBRARIES
	${SFML_GRAPHICS} ${SFML_WINDOW} ${SFML_SYSTEM}
	${JARLIB_CORE_NAME} ${JARLIB_INPUT_NAME}
	${JAR_LUA_NAME} ${JAR_LUABIND_NAME}
	)
set(JARLIB_AUDIO_LIBRARIES
	${SFML_AUDIO} ${SFML_SYSTEM}
	${JARLIB_CORE_NAME}
	${JAR_LUA_NAME} ${JAR_LUABIND_NAME}
	)

# defines
set(JARLIB_CORE_DEFINES "-DJAR_CORE_LIBRARY_BUILD")
set(JARLIB_INPUT_DEFINES "-DJAR_INPUT_LIBRARY_BUILD")
set(JARLIB_GRAPHICS_DEFINES "-DJAR_GRAPHICS_LIBRARY_BUILD")
set(JARLIB_AUDIO_DEFINES "-DJAR_AUDIO_LIBRARY_BUILD")

# add OS-specific files / libraries
if(WIN32)
	# Windows specific input stuff
	set(JARLIB_INPUT_FILES
		${JARLIB_INPUT_FILES}
		"src/jar/input/Windows/WinControllerDirectInput.cpp"
		"src/jar/input/Windows/WinControllerXInput.cpp"
		
		"include/jar/input/Windows/WinControllerDirectInput.hpp"
		"include/jar/input/Windows/WinControllerXInput.hpp"
		)
	# find DirectX SDK
	find_library(DXSDK_XINPUT_LIB
		NAMES xinput.lib
		PATH_SUFFIXES lib/x86 lib/x64
		PATHS ${DXSDK_DIR} $ENV{DXSDK_DIR})
	find_library(DXSDK_DINPUT_LIB
		NAMES dinput8.lib
		PATH_SUFFIXES lib/x86 lib/x64
		PATHS ${DXSDK_DIR} $ENV{DXSDK_DIR})
	find_library(DXSDK_DXGUID_LIB
		NAMES dxguid.lib
		PATH_SUFFIXES lib/x86 lib/x64
		PATHS ${DXSDK_DIR} $ENV{DXSDK_DIR})
	find_path(DXSDK_INCLUDE_DIR
		NAMES "d3d9.h"
		PATH_SUFFIXES "include"
		PATHS ${DXSDK_DIR} $ENV{DXSDK_DIR})
	if   ((NOT DXSDK_XINPUT_LIB) OR (NOT DXSDK_DINPUT_LIB) OR (NOT DXSDK_DXGUID_LIB) OR (NOT DXSDK_INCLUDE_DIR))
		message(FATAL_ERROR "Could not find DirectX SDK libraries (xinput.lib, dinput8.lib and dxguid.lib), is the DirectX SDK installed and DXSDK_DIR set?")
	endif((NOT DXSDK_XINPUT_LIB) OR (NOT DXSDK_DINPUT_LIB) OR (NOT DXSDK_DXGUID_LIB) OR (NOT DXSDK_INCLUDE_DIR))
	
	set(JARLIB_INPUT_LIBRARIES
		${JARLIB_INPUT_LIBRARIES}
		"${DXSDK_XINPUT_LIB}"
		"${DXSDK_DINPUT_LIB}"
		"${DXSDK_DXGUID_LIB}"
		"ole32"
		"oleaut32"
		)
	include_directories("${DXSDK_INCLUDE_DIR}")
	set(JARLIB_INPUT_DEFINES "${JARLIB_INPUT_DEFINES} -DWINVER=0x0500")
	
	# check for availability of wbemidl.h
	include(CheckIncludeFiles)
	check_include_files(wbemidl.h WBEMIDL_H_AVAILABLE)
	if(NOT WBEMIDL_H_AVAILABLE)
		find_path(MINGW_W64_INCLUDE_DIR
			"wbemidl.h"
			PATH_SUFFIXES "include" "mingw-w64-headers/include"
			PATHS ${MINGW_W64_ROOT} $ENV{MINGW_W64_ROOT}
			)
		if(NOT MINGW_W64_INCLUDE_DIR)
			message(FATAL_ERROR "wbemidl.h not available and MinGW-w64 not found, install it and set MINGW_W64_ROOT or make sure wbemidl.h is available, e.g. by using Visual Studio.")
		endif(NOT MINGW_W64_INCLUDE_DIR)
		message("Found wbemidl.h in mingw-w64 at ${MINGW_W64_INCLUDE_DIR}")
		include_directories("${MINGW_W64_INCLUDE_DIR}")
	endif(NOT WBEMIDL_H_AVAILABLE)
	
endif(WIN32)

# define the different libraries
add_library(${JARLIB_CORE_NAME} SHARED ${JARLIB_CORE_FILES})
add_library(${JARLIB_INPUT_NAME} SHARED ${JARLIB_INPUT_FILES})
add_library(${JARLIB_GRAPHICS_NAME} SHARED ${JARLIB_GRAPHICS_FILES})
add_library(${JARLIB_AUDIO_NAME} SHARED ${JARLIB_AUDIO_FILES})

target_link_libraries(${JARLIB_CORE_NAME} ${JARLIB_CORE_LIBRARIES})
target_link_libraries(${JARLIB_INPUT_NAME} ${JARLIB_INPUT_LIBRARIES})
target_link_libraries(${JARLIB_GRAPHICS_NAME} ${JARLIB_GRAPHICS_LIBRARIES})
target_link_libraries(${JARLIB_AUDIO_NAME} ${JARLIB_AUDIO_LIBRARIES})

set_target_properties(${JARLIB_CORE_NAME} PROPERTIES COMPILE_FLAGS ${JARLIB_CORE_DEFINES})
set_target_properties(${JARLIB_INPUT_NAME} PROPERTIES COMPILE_FLAGS ${JARLIB_INPUT_DEFINES})
set_target_properties(${JARLIB_GRAPHICS_NAME} PROPERTIES COMPILE_FLAGS ${JARLIB_GRAPHICS_DEFINES})
set_target_properties(${JARLIB_AUDIO_NAME} PROPERTIES COMPILE_FLAGS ${JARLIB_AUDIO_DEFINES})

if(WIN32)
	install(TARGETS ${JARLIB_CORE_NAME} ${JARLIB_INPUT_NAME} ${JARLIB_GRAPHICS_NAME} ${JARLIB_AUDIO_NAME}
		ARCHIVE DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/lib"
		RUNTIME DESTINATION "${JAR_BINARY_DIR}")
else(WIN32)
	install(TARGETS ${JARLIB_CORE_NAME} ${JARLIB_INPUT_NAME} ${JARLIB_GRAPHICS_NAME} ${JARLIB_AUDIO_NAME}
		LIBRARY DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/lib")
	install(TARGETS ${JARLIB_CORE_NAME} ${JARLIB_INPUT_NAME} ${JARLIB_GRAPHICS_NAME} ${JARLIB_AUDIO_NAME}
		LIBRARY DESTINATION "${JAR_BINARY_DIR}")
endif(WIN32)

message("${JARLIB_CORE_NAME} target generated.")
message("${JARLIB_INPUT_NAME} target generated.")
message("${JARLIB_GRAPHICS_NAME} target generated.")
message("${JARLIB_AUDIO_NAME} target generated.")
