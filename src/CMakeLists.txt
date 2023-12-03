# Root cmake
cmake_minimum_required(VERSION 3.22)

Project(RealTimeErosion VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()

# Copy files
set(DATA_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Data")
set(DESTINATION_COPY "${CMAKE_BINARY_DIR}/Erosion/Data")

# Engine
add_subdirectory(3rdParty)

# Game Project
add_subdirectory(Erosion)