cmake_minimum_required(VERSION 3.10)
project(SynthProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Find the SDL2 and SDL2_ttf packages
# This replaces manual -I and -L paths
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# 2. Define the executable and source files
add_executable(synth 
    Oscillator.cpp
    main.cpp 
)

# 3. Link the libraries
# On Linux, we don't need mingw32 or winmm!
target_link_libraries(synth PRIVATE 
    SDL2::SDL2 
    # SDL2::SDL2main is usually not needed on Linux, 
    # but SDL2::SDL2 handles the heavy lifting.
)