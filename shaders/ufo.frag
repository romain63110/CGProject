#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

uniform sampler2D diffuseTexture;
uniform vec3 lightColor;

void main()
{
    // =====================================================
    // DEBUG 1 — Shader utilisé ?
    // =====================================================
    // Si tu vois l'UFO ROUGE ? shader OK
    //FragColor = vec4(1, 0, 0, 1);
    //return;

    // =====================================================
    // DEBUG 2 — UVs valides ?
    // =====================================================
    // Doit afficher un dégradé coloré
    //FragColor = vec4(TexCoords, 0.0, 1.0);
    //return;

    // =====================================================
    // DEBUG 3 — Texture chargée ?
    // =====================================================
    vec4 texSample = texture(diffuseTexture, TexCoords);

    // Si l'UFO est noir ici ? texture non chargée / non bindée
    //FragColor = texSample;
    //return;

    // =====================================================
    // DEBUG 4 — Lumière seule (sans texture)
    // =====================================================
    vec3 ambient = 0.2 * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = spec * lightColor;

    vec3 lighting = ambient + diffuse + specular;

    // Si noir ici ? lightColor ou lightPos = 0
    //FragColor = vec4(lighting, 1.0);
    //return;

    // =====================================================
    // RENDU FINAL
    // =====================================================
    FragColor = vec4(lighting * texSample.rgb, 1.0);
}
