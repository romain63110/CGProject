#version 330 core
in vec3 vLocalPos;
out vec4 FragColor;

uniform float uTime;
uniform float uIntensity;

void main()
{
    float along = clamp(vLocalPos.z, 0.0, 1.0);

    along = 1.0 - along;

    float r = length(vLocalPos.xy);

    
    float radius = mix(1.1, 0.02, along);

    float body = smoothstep(radius, radius * 0.7, r);
    float core = smoothstep(radius * 0.45, 0.0, r);

    float tipFade = 1.0 - smoothstep(0.92, 1.0, along);

    float alpha = (0.35 * body + 1.2 * core) * tipFade;
    alpha *= uIntensity;

   
    alpha *= 40.0;
    alpha = clamp(alpha, 0.0, 1.0);

    vec3 col = vec3(10.0, 0.6, 0.1);

    FragColor = vec4(col, alpha);
}
