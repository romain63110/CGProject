#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float Height;
out vec3 FragPos;
out vec3 Normal;


// ======================================================
// Simplex 2D noise
// ======================================================
vec3 permute(vec3 x) { return mod(((x*34.0)+1.0)*x, 289.0); }

float snoise(vec2 v){
    const vec4 C = vec4(
        0.211324865405187,
        0.366025403784439,
       -0.577350269189626,
        0.024390243902439
    );

    vec2 i  = floor(v + dot(v, C.yy));
    vec2 x0 = v - i + dot(i, C.xx);

    vec2 i1 = (x0.x > x0.y) ? vec2(1.0,0.0) : vec2(0.0,1.0);

    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;

    i = mod(i, 289.0);
    vec3 p = permute(
        permute(i.y + vec3(0.0, i1.y, 1.0))
      + i.x + vec3(0.0, i1.x, 1.0)
    );

    vec3 m = max(
        0.5 - vec3(
            dot(x0,x0),
            dot(x12.xy,x12.xy),
            dot(x12.zw,x12.zw)
        ), 0.0
    );

    m = m*m;
    m = m*m;

    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;

    m *= 1.79284291400159 - 0.85373472095314 * (a0*a0 + h*h);

    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;

    return 130.0 * dot(m, g);
}


// ======================================================
// Hash stable par zone
// ======================================================
float hash12(vec2 p)
{
    vec3 p3 = fract(vec3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}


// ======================================================
// Courbe collines (écrase les pentes)
// ======================================================
float hillsCurve(float n)
{
    float t = n * 0.5 + 0.5;
    t = smoothstep(0.0, 1.0, t);
    t = smoothstep(0.0, 1.0, t);
    return t * 2.0 - 1.0;
}


// ======================================================
// Hauteur terrain
// ======================================================
float getNoiseHeight(float x, float z)
{
    // -----------------------------
    // Amplitude variable par zone
    // -----------------------------
    float ampZoneScale = 0.00035;
    vec2 zoneId = floor(vec2(x, z) * ampZoneScale);
    float zoneAmp = mix(140.0, 280.0, hash12(zoneId));

    // -----------------------------
    // Macro collines larges
    // -----------------------------
    float macroScale = 0.0011;
    float macro = snoise(vec2(x, z) * macroScale);
    macro = hillsCurve(macro);
    macro *= 0.7;

    float baseHeight = macro * zoneAmp;

    // -----------------------------
    // Variations secondaires (très douces)
    // -----------------------------
    float midScale = 0.004;
    float midNoise = snoise(vec2(x, z) * midScale);

    baseHeight += midNoise * 25.0;

    // -----------------------------
    // Détails légers uniquement en altitude
    // -----------------------------
    float detailScale = 0.01;
    float detailNoise = snoise(vec2(x, z) * detailScale);

    float detailMask = smoothstep(80.0, 220.0, abs(baseHeight));
    float finalH = baseHeight + detailNoise * 10.0 * detailMask;

    // ==================================================
    // ZONE DE SPAWN RECTANGULAIRE (X étroit / Z long)
    // ==================================================

    // Dimensions du rectangle
    float halfWidthX  = 40.0;   // largeur en X
    float halfLengthZ = 1000.0;  // longueur en Z (étiré)

    // Distance normalisée au rectangle
    float dx = abs(x) / halfWidthX;
    float dz = abs(z) / halfLengthZ;

    float rectDist = max(dx, dz);

    // Transition douce
    float spawnMask = smoothstep(0.8, 1.0, rectDist);

    // Aplat complet dans la zone
    finalH = mix(-1.0, finalH, spawnMask);

    // -----------------------------
    // Lac / niveau minimum
    // -----------------------------
    finalH = max(finalH, -5.0);

    return finalH;
}


// ======================================================
// Main
// ======================================================
void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);

    float h = getNoiseHeight(worldPos.x, worldPos.z);
    worldPos.y = h;

    float e = 0.1;
    float hx = getNoiseHeight(worldPos.x + e, worldPos.z);
    float hz = getNoiseHeight(worldPos.x, worldPos.z + e);

    vec3 tangentX = normalize(vec3(e, hx - h, 0.0));
    vec3 tangentZ = normalize(vec3(0.0, hz - h, e));

    Normal = normalize(cross(tangentZ, tangentX));

    Height = h;
    FragPos = vec3(worldPos);
    gl_Position = projection * view * worldPos;
}
