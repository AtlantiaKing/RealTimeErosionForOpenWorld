# Subengine CMake
set(LEAP_PHYSICS_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE PATH "")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(LEAP_PHYSICS_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/PhysicsEngine.lib" CACHE PATH "")
else()
	set(LEAP_PHYSICS_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/release/PhysicsEngine.lib" CACHE PATH "")
endif()