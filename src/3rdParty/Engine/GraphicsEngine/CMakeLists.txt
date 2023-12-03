# Subengine CMake
set(LEAP_GRAPHICS_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE PATH "")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(LEAP_GRAPHICS_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/GraphicsEngine.lib" CACHE PATH "")
else()
	set(LEAP_GRAPHICS_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/release/GraphicsEngine.lib" CACHE PATH "")
endif()