#version 330 core
out vec4 FragColor;

in float Height;
in vec3 FragPos;
in vec3 Normal;

// Texture
uniform sampler2D textureSand;
uniform sampler2D textureGrass;
uniform sampler2D textureRock;
uniform sampler2D textureSnow;

void main()
{
    vec2 uv = FragPos.xz * 0.05; //0.05 the textures appear larger

    // get color for each texture
    vec3 colSand  = texture(textureSand, uv).rgb;
    vec3 colGrass = texture(textureGrass, uv).rgb;
    vec3 colRock  = texture(textureRock, uv).rgb;
    vec3 colSnow  = texture(textureSnow, uv).rgb;

    vec3 finalColor;

    // ? GRASS: commence plus bas et reste plus longtemps
    float grassWeight = smoothstep(-10.0, 5.0, Height);
    vec3 layer1 = mix(colSand, colGrass, grassWeight);

    // ? ROCK: commence plus haut (donc + de grass)
    float rockWeight = smoothstep(20.0, 60.0, Height);
    vec3 layer2 = mix(layer1, colRock, rockWeight);

    // ? SNOW: un peu plus haut pour garder plus de rock/grass
    float snowWeight = smoothstep(90.0, 130.0, Height);
    finalColor = mix(layer2, colSnow, snowWeight);

    // Light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 ambient = vec3(0.3) * finalColor;
    vec3 diffuse = diff * finalColor;

    FragColor = vec4(ambient + diffuse, 1.0);
}
