cmake_minimum_required(VERSION 3.10)

# Project Name
project(SoundComposer)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Find SDL2
find_package(SDL2 REQUIRED)

# Include directories
include_directories(src/audio)

# Add source files
add_executable(sound_composer 
    src/core/main.c 
    src/audio/util.c
)

# Link SDL2
target_link_libraries(sound_composer SDL2::SDL2)
