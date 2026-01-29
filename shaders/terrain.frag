#version 330 core
out vec4 FragColor;

in float Height;
in vec3 FragPos;
in vec3 Normal;

// ===== TEXTURES =====
uniform sampler2D textureSand;
uniform sampler2D textureGrass;
uniform sampler2D textureRock;
uniform sampler2D textureSnow;

// ===== FOG =====
uniform vec3 cameraPos;
uniform vec3 fogColor;
uniform float fogDensity;

void main()
{
    // ======================
    // TEXTURE BLENDING
    // ======================
    vec2 uv = FragPos.xz * 0.05;

    vec3 sand  = texture(textureSand,  uv).rgb;
    vec3 grass = texture(textureGrass, uv).rgb;
    vec3 rock  = texture(textureRock,  uv).rgb;
    vec3 snow  = texture(textureSnow,  uv).rgb;

    float wGrass = smoothstep(-10.0, 5.0, Height);
    vec3 layer1 = mix(sand, grass, wGrass);

    float wRock = smoothstep(20.0, 60.0, Height);
    vec3 layer2 = mix(layer1, rock, wRock);

    float wSnow = smoothstep(90.0, 130.0, Height);
    vec3 baseColor = mix(layer2, snow, wSnow);


    vec3 N = normalize(Normal);
    vec3 L = normalize(vec3(0.5, 1.0, 0.3));

    float diff = max(dot(N, L), 0.0);

    vec3 ambient = baseColor * 0.3;
    vec3 diffuse = baseColor * diff;

    vec3 litColor = ambient + diffuse;

    float dist = length(FragPos - cameraPos);

    float fogStart = 2200;
    dist = max(dist - fogStart, 0.0);

    // Fog exponentiel (réaliste)
    float fogFactor = exp(-dist * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    // Fog plus dense près du sol
    float heightFog = smoothstep(0.0, 10.0, FragPos.y);
    fogFactor *= heightFog;


    vec3 finalColor = mix(fogColor, litColor, fogFactor);
    FragColor = vec4(finalColor, 1.0);
}
