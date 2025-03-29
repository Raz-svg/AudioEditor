cmake_minimum_required(VERSION 3.10)
project(wav_viewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)

# Add source files
add_executable(wav_viewer
    src/core/main.cpp
    src/audio/audio.cpp
    src/ui/ui.cpp
)

# Add include directories
target_include_directories(wav_viewer PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Link SDL2
target_link_libraries(wav_viewer PRIVATE SDL2::SDL2) 