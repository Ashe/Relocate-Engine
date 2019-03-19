cmake_minimum_required (VERSION 3.7)
project (Game)

message("Using CMake version ${CMAKE_VERSION}")
message("Processing CMakeLists.txt")

# Program variables
set (Build_VERSION_MAJOR 0)
set (Build_VERSION_MINOR 0)
set (Build_VERSION_TWEAK 21)

# Configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/src/Config.h.in"
  "${PROJECT_SOURCE_DIR}/src/Config.h"
)

# Compiler version
set (CMAKE_CXX_STANDARD 17)

# Define location for modules
set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules/)
message("Module path: ${CMAKE_MODULE_PATH}")

# If on Windows, specify where these packages are installed
if (WIN32)
  set(SFML_STATIC_LIBRARIES TRUE)
  set(SFML_DIR "C:/Program Files (x86)/SFML-2.5.1/lib/cmake/SFML")
endif()

# Find X11
find_package(X11)

# Find multithreading package
find_package(Threads REQUIRED)

# Find SFML
find_package(SFML 2.5.1 REQUIRED COMPONENTS window system graphics audio network)

# Find Box2D
set(BOX2D_STATIC_LIBRARIES TRUE)
find_package(Box2D REQUIRED)
include_directories(${BOX2D_INCLUDE_DIR})

# Find lua
find_package(Lua REQUIRED)
include_directories(${LUA_INCLUDE_DIR})

# Create the executable
set(EXECUTABLE_NAME ${PROJECT_NAME})
add_executable (${EXECUTABLE_NAME} src/main.cpp)

# Add sources
target_sources(${EXECUTABLE_NAME} PRIVATE
  src/Config.h
  src/ECS.h
  src/Sol.h
  src/Scripting.h
  src/Scripting.cpp
  src/Game.h
  src/Game.cpp
  src/Scene.h
  src/Scene.cpp

  src/Transform.h
  src/RigidBody.h
  src/RigidBody.cpp

  src/PhysicsSystem.h
  src/PhysicsSystem.cpp
  src/PhysicsDebugDraw.h
  src/PhysicsDebugDraw.cpp
)

# Platform specific building
if (WIN32)
  # Link libraries so that they can be used in the project
  target_link_libraries(${EXECUTABLE_NAME}
    sfml-window
    sfml-system
    sfml-graphics
    sfml-audio
    sfml-network
    ${BOX2D_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LUA_LIBRARIES}
  )
endif()
if(UNIX)
  # Link libraries so that they can be used in the project
  target_link_libraries(${EXECUTABLE_NAME}
    sfml-window
    sfml-system
    sfml-graphics
    sfml-audio
    sfml-network
    lua
    X11
    Box2D
  )
endif()

# Link libraries so that they can be used in the project
target_link_libraries(${EXECUTABLE_NAME}
  sfml-window
  sfml-system
  sfml-graphics
  sfml-audio
  sfml-network
  ${X11_LIBRARIES}
  ${BOX2D_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${LUA_LIBRARIES}
)

# Copy assets
file(COPY ${CMAKE_SOURCE_DIR}/Assets DESTINATION ${CMAKE_BINARY_DIR})
