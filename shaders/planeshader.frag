#version 330 core

in vec3 vNormal;
in vec3 vFragPos;

out vec4 FragColor;

uniform vec3 uKd;
uniform vec3 uKs;
uniform float uNs;

void main()
{
    vec3 N = normalize(vNormal);
    vec3 L = normalize(vec3(1.0, 1.0, 1.0));
    vec3 V = normalize(vec3(0.0, 0.0, 1.0));

    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = uKd * diff;

    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(V, R), 0.0), uNs);
    vec3 specular = uKs * spec;

    vec3 ambient = uKd * 3;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
