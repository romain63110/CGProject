#version 330 core

out vec4 FragColor;
in vec3 LocalPos;

//looked into alpha blending

void main() {
    float dist = length(LocalPos);
    float alpha = 1.0 - (dist/1.2); 
    alpha = pow(alpha, 2.0);
    alpha = clamp(alpha, 0.0, 1.0);

    FragColor = vec4(1.0, 1.0, 1.0, alpha * 0.8); //blending
}