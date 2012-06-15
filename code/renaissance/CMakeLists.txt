cmake_minimum_required(VERSION 2.6)

message("\n= Renaissance =\n")

if(NOT JAR_BINARY_DIR)
	message(FATAL_ERROR "Use the toplevel cmake script!")
endif(NOT JAR_BINARY_DIR)

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