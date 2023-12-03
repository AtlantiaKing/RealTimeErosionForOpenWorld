# Subengine CMake
set(LEAP_NETWORK_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE PATH "")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(LEAP_NETWORK_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/NetworkingEngine.lib" CACHE PATH "")
else()
	set(LEAP_NETWORK_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/release/NetworkingEngine.lib" CACHE PATH "")
endif()