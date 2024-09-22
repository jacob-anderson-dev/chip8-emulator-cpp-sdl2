cmake_minimum_required(VERSION 3.10)

# Set the project name and C++ standard
project(MyProject VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -ggdb")

# Use pkg-config to find SDL2
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

# Include directories and link libraries for both SDL2
include_directories(${SDL2_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS})

# Define directories
set(SOURCE_DIR src)
set(INCLUDE_DIR include)
set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)

# Collect sources
file(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

# Define executable output
add_executable(${PROJECT_NAME} ${SOURCES})

# Set output directory for executable
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)

# Link SDL2 libraries
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

# Create a custom clean target that deletes the binaries
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${BIN_DIR}
)