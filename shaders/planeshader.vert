#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

out vec3 vNormal;
out vec3 vFragPos;

uniform vec4 plane;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vFragPos = vec3(model * vec4(aPos, 1.0));
    vNormal  = mat3(transpose(inverse(model))) * aNormal;

    gl_ClipDistance[0] = dot(model * vec4(aPos, 1.0), plane);//OpenGL don't draw if < 0

    gl_Position = projection * view * vec4(vFragPos, 1.0);
}
