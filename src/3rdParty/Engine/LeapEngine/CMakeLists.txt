# Subengine CMake
set(LEAP_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE PATH "")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(LEAP_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/LeapEngine.lib" CACHE PATH "")
else()
	set(LEAP_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/release/LeapEngine.lib" CACHE PATH "")
endif()