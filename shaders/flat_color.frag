#version 330 core

in vec3 vWorldPos;
out vec4 FragColor;

uniform vec3 uColor;

void main()
{
    float stripeFreq = 0.05; // + petit = bandes plus espacées
    float stripe = step(0.5, fract(vWorldPos.z * stripeFreq));

    vec3 c1 = uColor;
    vec3 c2 = uColor * 0.55;

    vec3 col = mix(c1, c2, stripe);

    FragColor = vec4(col, 1.0);
}
