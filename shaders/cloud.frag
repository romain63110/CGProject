#version 330 core

out vec4 FragColor;
in vec3 LocalPos;

void main()
{
    // NORMALISATION CRUCIALE
    float dist = length(LocalPos);

    // Rayon logique ? 1.0
    float alpha = 1.0 - dist;
    alpha = clamp(alpha, 0.0, 1.0);

    // Courbe douce
    alpha = pow(alpha, 2.0);

    FragColor = vec4(vec3(1.0), alpha * 0.8);
}
