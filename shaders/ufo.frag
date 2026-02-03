#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;   // extra in variable, since we need the light position in view space we calculate this in the vertex shader


uniform sampler2D ufoTexture;
uniform vec3 lightColor;

//phong lighting model

void main() {
    vec4 textSample = texture(ufoTexture, TexCoords);
    vec3 rgbSample = textSample.rgb;

    // ambient   Ia=ia*Ka
    float ambiantStrength = 0.1;
    vec3 ambient = lightColor*ambiantStrength;  

    // diffuse  Id=id*Kd*(L.N)  with L = direction vector that points from the fragment to the light and N normal vector
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir),0.0); 
    vec3 diffuse = diff * lightColor;

    
    // specular
    float specularStrength = 1; //max shining effect on metal 
    vec3 viewDir = normalize(-FragPos); 
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = lightColor * spec * specularStrength;   
    
    vec3 result = (specular + diffuse + ambient)*rgbSample; 
    
    //if ufo texture missing show a black color
    FragColor = vec4(result, 1.0);
}
