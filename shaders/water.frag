#version 330 core
out vec4 FragColor;

in vec4 clipSpace;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;

void main()
{
    //Homogene coordinate -> NDC (Normalized Device Coordinates) -> Texture Coords [0, 1]
    vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;
    
    // tex coordonate
    vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
    vec2 refractTexCoords = vec2(ndc.x, ndc.y);
    

    // Read generated texture
    vec4 reflectColor = texture(reflectionTexture, reflectTexCoords);
    vec4 refractColor = texture(refractionTexture, refractTexCoords);

    // TODO: Fresnel
    FragColor = mix(reflectColor, refractColor, 0.5);
    
    // add blue (0.2)
    FragColor = mix(FragColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2);
}