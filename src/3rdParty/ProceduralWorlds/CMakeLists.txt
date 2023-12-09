# ProceduralWorlds CMake
add_library(ProceduralWorlds "Generator.cpp" "SuccessPredicate.cpp" "Heightmap/Heightmap.cpp" "Noise/Graph.cpp" "Noise/NoiseMap.cpp" "Noise/PerlinComposition.cpp" "Presets/Presets.cpp" "WorldShape/CirclePeak.cpp" "WorldShape/SquarePeak.cpp")
set(PROCWORLDS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "")