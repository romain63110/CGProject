cmake_minimum_required(VERSION 3.5)

project(OpenGLProgram)

set(CMAKE_CXX_STANDARD 17)

# Set directories
set(SRC_DIR src)
set(INCLUDE_DIR include)
set(EXTERNAL_DIR external)
set(GLEW_DIR ${EXTERNAL_DIR}/glew-cmake)
set(GLFW_DIR ${EXTERNAL_DIR}/glfw)
set(GLM_DIR ${EXTERNAL_DIR}/glm)

# Include directories
include_directories(${INCLUDE_DIR})
include_directories(${GLEW_DIR}/include)
include_directories(${GLFW_DIR}/include)
include_directories(${GLM_DIR})

# Source files
set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/node.cpp
    ${SRC_DIR}/shape.cpp
    ${SRC_DIR}/shader.cpp
    ${SRC_DIR}/viewer.cpp 
    ${SRC_DIR}/texture.cpp
    ${SRC_DIR}/skybox.cpp
    ${SRC_DIR}/terrain.cpp
    ${SRC_DIR}/terrain_chunk.cpp
)

# Define Shader director
add_compile_definitions(SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders/")

# GLEW library
add_subdirectory(${GLEW_DIR})
set(LIBS ${LIBS} libglew_static)

# GLFW library
add_subdirectory(${GLFW_DIR})
set(LIBS ${LIBS} glfw)

# GLM library
add_subdirectory(${GLM_DIR})

# Executable
add_executable(opengl_program ${SOURCES})
target_link_libraries(opengl_program ${LIBS})