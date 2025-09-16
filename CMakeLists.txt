cmake_minimum_required(VERSION 3.25)
project(Clash_of_Colosseum)

set(CMAKE_CXX_STANDARD 20)

# SDL2
find_package(SDL2 REQUIRED)

# SDL2_gfx
find_path(SDL2_GFX_INCLUDE_DIR SDL2/SDL2_gfxPrimitives.h)
find_library(SDL2_GFX_LIBRARIES NAMES SDL2_gfx)

# SDL2_image
find_path(SDL2_IMAGE_INCLUDE_DIR SDL2/SDL_image.h)
find_library(SDL2_IMAGE_LIBRARIES NAMES SDL2_image)

# SDL2_ttf
find_path(SDL2_TTF_INCLUDE_DIR SDL2/SDL_ttf.h)
find_library(SDL2_TTF_LIBRARIES NAMES SDL2_ttf)

# Include directories
include_directories(
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_GFX_INCLUDE_DIR}
        ${SDL2_IMAGE_INCLUDE_DIR}
        ${SDL2_TTF_INCLUDE_DIR}
)

# Tous les fichiers source
file(GLOB_RECURSE sourceCode src/*.cpp src/*.h)

# Crée l'exécutable
add_executable(Clash_of_Colosseum ${sourceCode})

# Link libraries
target_link_libraries(Clash_of_Colosseum
        ${SDL2_LIBRARIES}
        ${SDL2_GFX_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
)