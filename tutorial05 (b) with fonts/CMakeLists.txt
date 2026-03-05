cmake_minimum_required(VERSION 3.10)
project(SynthProject)

set(CMAKE_CXX_STANDARD 17)

# 1. Use PkgConfig to find SDL2 and SDL2_ttf
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

# 2. Define executable
add_executable(synth main.cpp Oscillator.cpp)

# 3. Link using the variables PkgConfig created
target_include_directories(synth PRIVATE ${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})
target_link_libraries(synth PRIVATE ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})