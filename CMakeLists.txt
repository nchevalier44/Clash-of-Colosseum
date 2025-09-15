cmake_minimum_required(VERSION 3.25)
project(Clash_of_Colosseum)

set(CMAKE_CXX_STANDARD 20)

# SDL2 et SDL2_ttf
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
message(STATUS "SDL2_ttf_INCLUDE_DIRS=${SDL2_TTF_INCLUDE_DIRS}")
message(STATUS "SDL2_ttf_LIBRARIES=${SDL2_TTF_LIBRARIES}")

# SDL2_gfx
find_path(SDL2_GFX_INCLUDE_DIR SDL2/SDL2_gfxPrimitives.h)
find_library(SDL2_GFX_LIBRARY NAMES SDL2_gfx)

# Include directories
include_directories(
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${SDL2_GFX_INCLUDE_DIR}
)

# Tous les fichiers source
file(GLOB_RECURSE sourceCode src/*.cpp src/*.h)

# Crée l'exécutable
add_executable(Clash_of_Colosseum ${sourceCode})

# Link libraries
target_link_libraries(Clash_of_Colosseum
        ${SDL2_LIBRARIES}
        SDL2_ttf
        ${SDL2_GFX_LIBRARY}
)
