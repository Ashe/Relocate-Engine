cmake_minimum_required (VERSION 3.7)
project (Game)

message("Using CMake version ${CMAKE_VERSION}")
message("Processing CMakeLists.txt")

# Program variables
set (Build_VERSION_MAJOR 0)
set (Build_VERSION_MINOR 0)
set (Build_VERSION_TWEAK 3)

# Configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/src/Config.h.in"
  "${PROJECT_SOURCE_DIR}/src/Config.h"
)

# Compiler version
set (CMAKE_CXX_STANDARD 17)

# Find SFML
find_package(SFML 2 REQUIRED COMPONENTS window system graphics audio network)

# Create the executable
set(EXECUTABLE_NAME ${PROJECT_NAME})
add_executable (${EXECUTABLE_NAME} src/main.cpp)

# Add sources
target_sources(${EXECUTABLE_NAME} PRIVATE
  src/Config.h
  src/Game.h
  src/Game.cpp
  src/Screen.h
  src/ECS.h
  src/GameScreen.h
  src/GameScreen.cpp
)

# Link libraries so that they can be used in the project
target_link_libraries(${EXECUTABLE_NAME}
  sfml-window
  sfml-system
  sfml-graphics
  sfml-audio
  sfml-network
)

# Copy assets
file(COPY ${CMAKE_SOURCE_DIR}/Assets DESTINATION ${CMAKE_BINARY_DIR})
