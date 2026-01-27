cmake_minimum_required(VERSION 3.5)

project(OpenGLProgram)

set(SRC_DIR src)
set(INCLUDE_DIR include)
set(EXTERNAL_DIR external)
set(GLEW_DIR ${EXTERNAL_DIR}/glew-cmake)
set(GLFW_DIR ${EXTERNAL_DIR}/glfw)
set(GLM_DIR ${EXTERNAL_DIR}/glm)
set(IMGUI_DIR ${EXTERNAL_DIR}/imgui)

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
    ${SRC_DIR}/water.cpp
    ${SRC_DIR}/water_framebuffers.cpp
)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_compile_definitions(SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders/")

add_subdirectory(${GLEW_DIR})
add_subdirectory(${GLFW_DIR})
add_subdirectory(${GLM_DIR})

add_executable(opengl_program ${SOURCES} ${IMGUI_SOURCES} "include/aircraft.h" "src/aircraft.cpp" "include/aircraft_controls.h" "src/aircraft_controls.cpp" "include/flight_model_physics.h" "src/flight_model_physics.cpp" "include/camera_controller.h" "src/camera_controller.cpp" "include/runway.h" "src/runway.cpp" "include/obj_model.h" "src/obj_model.cpp"   "include/afterburner_flame.h" "src/afterburner_flame.cpp")

target_include_directories(opengl_program PRIVATE
    ${INCLUDE_DIR}
    ${GLEW_DIR}/include
    ${GLFW_DIR}/include
    ${GLM_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(opengl_program PRIVATE
    libglew_static
    glfw
)
