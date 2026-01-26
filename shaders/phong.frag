#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightDir;    // direction de la lumière (ex: normalize(vec3(-1,-1,-1)))
uniform vec3 viewPos;     // position de la caméra

// Couleur du matériau (vert)
uniform vec3 objectColor; // ex: vec3(0.1, 0.8, 0.2)
uniform vec3 lightColor;  // ex: vec3(1.0)

void main()
{
    // Ambient
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir); // direction vers la lumière
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.6;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
