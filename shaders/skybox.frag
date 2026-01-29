#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 fogColor;
uniform float fogStrength;

void main()
{
    vec3 skyColor = texture(skybox, TexCoords).rgb;

    // TexCoords.y ? [-1, 1]
    // Horizon ? 0 ? plus de fog
    float horizonFactor = clamp(1.0 - max(TexCoords.y, 0.0), 0.0, 1.0);

    // Fog progressif
    float fog = horizonFactor * fogStrength;

    vec3 finalColor = mix(skyColor, fogColor, fog);

    FragColor = vec4(finalColor, 1.0);
}
