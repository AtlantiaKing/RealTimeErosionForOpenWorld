# Subengine CMake
set(LEAP_INPUT_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE PATH "")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(LEAP_INPUT_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/InputEngine.lib" CACHE PATH "")
else()
	set(LEAP_INPUT_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/release/InputEngine.lib" CACHE PATH "")
endif()