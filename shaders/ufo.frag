#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D ufoTexture;
uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{
    // --- Texture ---
    vec4 texSample = texture(ufoTexture, TexCoords);
    vec3 albedo = texSample.rgb;

    // Sécurité : texture absente
    if (texSample.a < 0.01)
        discard;

    // --- Ambient ---
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // --- Diffuse ---
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // --- Specular ---
    float specularStrength = 0.5;
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;

    // --- Final color ---
    vec3 result = (ambient + diffuse + specular) * albedo;
    FragColor = vec4(result, 1.0);
}
