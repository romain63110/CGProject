#version 330 core

layout(location = 0) in vec3 aPos;

uniform vec4 plane;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vWorldPos;

void main()
{
    vec4 world = model * vec4(aPos, 1.0);
    gl_ClipDistance[0] = dot(world, plane);//OpenGL don't draw if < 0

    vWorldPos = world.xyz;

    gl_Position = projection * view * world;
}
