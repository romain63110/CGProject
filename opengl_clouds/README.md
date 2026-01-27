# Clouds and UFO rendering through Opengl 

## Project Overview
This project is an interactive map application that allows users to navigate and explore different areas. It includes features like zooming, clickable areas, and tooltips with additional information.

## Files  Included (important code)
- `src/` folder contains `cloud.cpp`, `main.cpp`, `mesh.cpp` and `viewer.cpp`
- `include/` folder contains `cloud.h`, `mesh.h`, `stb_image.h` and `viewer.h`
- `shaders/` folder contains `cloud.frag`, `cloud.vert` and `ufo.frag`, `ufo.vert` and `skybox.frag`, `skybox.vert`
- `texture`contains all images, for the skybox and UFO texture 
- `cloud.obj` and `UFO.obj` are 2 imported files from blender 

## Main idea 

Mesh.cpp function loads the obj file using assimp into opengl vertices, that are then handled in cloud.cpp. VAO and VBO are created, the position, angle and texture of the object is also specified (initCloudField can generate bothe clouds and UFOs).
Viewer handles everything to do with the animation, skybox creation, shaders and camera positions. 
Shaders contain all objects transformations and visual effects (color, texture)
Main finally calls all that and renders all three objects, Skybox, CLouds and UFO

## How To Run 
1. Compile :
    cmake .
    make
    ./opengl_program
3. Can move camera anlges by mouse 
4. Can move forwards, backwards then can go up and down
5. Enjoy!

   

