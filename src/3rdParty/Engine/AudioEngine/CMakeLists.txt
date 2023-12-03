# Subengine CMake
set(LEAP_AUDIO_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE PATH "")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(LEAP_AUDIO_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/AudioEngine.lib" CACHE PATH "")
else()
	set(LEAP_AUDIO_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/release/AudioEngine.lib" CACHE PATH "")
endif()