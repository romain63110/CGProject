#version 330 core
out vec4 FragColor;

in vec4 clipSpace;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;


uniform vec3 fogColor;
uniform float fogStart = 0.4f; 
uniform float fogEnd = 1.4f;

void main()
{
  
    vec2 ndc = (clipSpace.xy / clipSpace.w) * 0.5 + 0.5;

    vec2 reflectUV = vec2(ndc.x, 1.0 - ndc.y);
    vec2 refractUV = ndc;

    vec4 reflectColor = texture(reflectionTexture, reflectUV);
    vec4 refractColor = texture(refractionTexture, refractUV);

 
    vec4 waterColor = mix(reflectColor, refractColor, 0.5);
    waterColor = mix(waterColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2);

    float dist = abs(clipSpace.z / clipSpace.w);


    float fogFactor = smoothstep(fogStart, fogEnd, dist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    FragColor = mix(waterColor, vec4(fogColor, 1.0), fogFactor);
}
