cmake_minimum_required(VERSION 3.5)

project(OpenGLProgram)

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
    ${SRC_DIR}/triangle.cpp
    ${SRC_DIR}/pyramid.cpp
    ${SRC_DIR}/cylinder.cpp
    ${SRC_DIR}/sphere.cpp
    ${SRC_DIR}/texture.cpp
    ${SRC_DIR}/textured_sphere.cpp
    ${SRC_DIR}/lighting_sphere.cpp
    ${SRC_DIR}/skybox.cpp
)

# Define Shader director
add_compile_definitions(SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders/")

# GLEW library
#add_subdirectory(${GLEW_DIR})
# GLEW library (Linux: system, Windows: bundled)
if (WIN32)
    add_subdirectory(${GLEW_DIR})
    set(GLEW_LIB libglew_static)
else()
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    set(GLEW_LIB GLEW::GLEW)
endif()


# GLFW library
add_subdirectory(${GLFW_DIR})

# GLM library
# DO NOT ADD_SUBDIRECTORY — GLM is header-only
# include directories instead
include_directories(${GLM_DIR})

# Executable
add_executable(opengl_program ${SOURCES})

# Link libraries
target_link_libraries(opengl_program PRIVATE
    ${GLEW_LIB}
    glfw
    ${OPENGL_LIBRARIES}
)

