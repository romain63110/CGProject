#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos; // we now define the uniform in the vertex shader and pass the 'view space' lightpos to the fragment shader. lightPos is currently in world space.

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec3 LightPos;


void main() {
    TexCoords=aTexCoords;

    vec4 viewPos = view * model * vec4(aPos, 1.0);
    FragPos = viewPos.xyz;

    // 2. Transform Normal to View Space
    // We use the Normal Matrix (inverse transpose) to keep normals perpendicular during scaling
    Normal = mat3(transpose(inverse(view * model))) * aNormal;

    // 3. Transform Light to View Space
    LightPos = vec3(view * vec4(lightPos, 1.0));

    // 4. Standard clip-space output
    gl_Position = projection * viewPos;
    
    //world space
    //Normal = mat3(transpose(inverse(model))) * aNormal;

    //world position
    /*
    FragPos = vec3(model * vec4(aPos, 1.0));

    LightPos = vec3(view * vec4(lightPos, 1.0)); // Transform world-space light position to view-space light position

    gl_Position = projection * view * model * vec4(aPos, 1.0);*/
}
