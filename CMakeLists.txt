cmake_minimum_required(VERSION 3.15)
project(BoingBall)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Find OpenAL
find_library(OPENAL_LIBRARY OpenAL)
if(NOT OPENAL_LIBRARY)
    message(FATAL_ERROR "OpenAL library not found")
endif()

# Include directories for headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/shader.cpp
    src/sphere.cpp
    src/texture.cpp
    src/physics.cpp
    src/sound.cpp
    src/cube.cpp
    src/tracer.cpp
    include/glad.c
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
    ${OPENAL_LIBRARY}
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
    "-framework AudioToolbox"
    "-framework AudioUnit"
    "-framework CoreAudio"
)

# Copy shaders and resources to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sounds DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
