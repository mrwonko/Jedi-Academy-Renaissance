cmake_minimum_required(VERSION 2.6)

message("\n= Renaissance =\n")

if(NOT JAR_BINARY_DIR)
	message(FATAL_ERROR "Use the toplevel cmake script!")
endif(NOT JAR_BINARY_DIR)



if(NOT SFML_FOUND)
	find_package(SFML 2 REQUIRED system window graphics audio)
endif(NOT SFML_FOUND)
include_directories("${SFML_INCLUDE_DIR}")
add_definitions(-DSFML_DYNAMIC) # this may be outdated? there's SFML_STATIC, this may be assumed otherwise...

if(NOT Boost_FOUND)
	find_package(Boost REQUIRED)
endif(NOT Boost_FOUND)
include_directories(${Boost_INCLUDE_DIRS})

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
	set(JAR_RENAISSANCE_NAME "renaissance_d")
else(${CMAKE_BUILD_TYPE} MATCHES Debug)
	set(JAR_RENAISSANCE_NAME "renaissance")
endif(${CMAKE_BUILD_TYPE} MATCHES Debug)

add_executable(${JAR_RENAISSANCE_NAME}
	"src/main.cpp" 
	"include/SimpleLevel.hpp"
	"src/SimpleLevel.cpp"
	"src/LBSimpleLevel.cpp"
	)

include_directories("include")

target_link_libraries(${JAR_RENAISSANCE_NAME}
	${JARLIB_PHYSICS_NAME} ${JARLIB_GRAPHICS_NAME} ${JARLIB_INPUT_NAME} ${JARLIB_AUDIO_NAME} ${JARLIB_CORE_NAME}
	${BULLET_DYNAMICS_LIBRARY} ${BULLET_COLLISION_LIBRARY} ${BULLET_MATH_LIBRARY}
	${JAR_LUA_NAME} ${JAR_LUABIND_NAME}
	)

install(TARGETS ${JAR_RENAISSANCE_NAME}
	RUNTIME DESTINATION "${JAR_BINARY_DIR}")

message("${JAR_RENAISSANCE_NAME} target generated.")