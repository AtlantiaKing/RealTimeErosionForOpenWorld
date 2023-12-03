# Subengine CMake
set(LEAP_UTILS_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE PATH "")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(LEAP_UTILS_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/EngineUtils.lib" CACHE PATH "")
else()
	set(LEAP_UTILS_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/release/EngineUtils.lib" CACHE PATH "")
endif()