cmake_minimum_required(VERSION 3.25)
project(Clash_of_Colosseum)

set(CMAKE_CXX_STANDARD 20)

find_path(SDL2_GFX_INCLUDE_DIR SDL2/SDL2_gfxPrimitives.h)
find_library(SDL2_GFX_LIBRARY NAMES SDL2_gfx)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_GFX_INCLUDE_DIR})

file(GLOB_RECURSE sourceCode src/*.cpp src/*.h)
add_executable(Clash_of_Colosseum ${sourceCode})

target_link_libraries(Clash_of_Colosseum ${SDL2_LIBRARIES} ${SDL2_GFX_LIBRARY})
