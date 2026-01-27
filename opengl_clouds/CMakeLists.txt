cmake_minimum_required(VERSION 3.10)
project(OpenGLClouds)

cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(ASSIMP REQUIRED assimp)

add_executable(opengl_clouds src/main.cpp src/viewer.cpp src/cloud.cpp src/mesh.cpp)

target_link_libraries(opengl_clouds OpenGL::GL GLEW::GLEW glfw ${ASSIMP_LIBRARIES} dl pthread)

target_include_directories(opengl_clouds PUBLIC ${ASSIMP_INCLUDE_DIRS} include/)
