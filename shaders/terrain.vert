#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float heightScale;
uniform vec4 plane;

out float Height;
out vec3 FragPos;
out vec3 Normal;

vec3 permute(vec3 x) { return mod(((x*34.0)+1.0)*x, 289.0); }

float snoise(vec2 v){
  const vec4 C = vec4(0.211324865405187, 0.366025403784439,
           -0.577350269189626, 0.024390243902439);
  vec2 i  = floor(v + dot(v, C.yy));
  vec2 x0 = v - i + dot(i, C.xx);
  vec2 i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
  i = mod(i, 289.0);
  vec3 p = permute(permute(i.y + vec3(0.0, i1.y, 1.0)) + i.x + vec3(0.0, i1.x, 1.0));
  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m;
  m = m*m;
  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;
  m *= 1.79284291400159 - 0.85373472095314 * (a0*a0 + h*h);
  vec3 g;
  g.x  = a0.x * x0.x + h.x * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

float hash12(vec2 p)
{
    vec3 p3 = fract(vec3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

float roundPeaks(float n)
{
    float t = n * 0.5 + 0.5;
    t = smoothstep(0.0, 1.0, t);
    return t * 2.0 - 1.0;
}

float getNoiseHeight(float x, float z) 
{
    float zoneScale = 0.002;
    float amplitudeZoneScale = 0.0004;

    vec2 zoneId = floor(vec2(x, z) * amplitudeZoneScale);
    float randA = hash12(zoneId);
    float zoneAmplitude = mix(150.0, 300.0, randA);

    float macroNoise = snoise(vec2(x * zoneScale, z * zoneScale));
    float base = roundPeaks(macroNoise);
    base = sign(base) * pow(abs(base), 1.15);
    float baseHeight = base * zoneAmplitude;

    float detailScale = 0.02;
    float detailNoise = snoise(vec2(x * detailScale, z * detailScale));
    float altitudeFactor = smoothstep(30.0, 140.0, abs(baseHeight));

    float finalH = baseHeight + (detailNoise * 1.5 * altitudeFactor);

    float distFromCenter = length(vec2(x, z));
    float plainRadius = 1600.0;
    float plainMask = smoothstep(plainRadius * 0.7, plainRadius, distFromCenter);

    finalH = mix(-5.0, finalH, plainMask);

    if (finalH < -5.0)
        finalH = -5.0;

    return finalH - 10.0;
}

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);

    float h = getNoiseHeight(worldPos.x, worldPos.z);
    worldPos.y = h;

    gl_ClipDistance[0] = dot(worldPos, plane);

    float e = 0.1;
    float h_right = getNoiseHeight(worldPos.x + e, worldPos.z);
    float h_back  = getNoiseHeight(worldPos.x, worldPos.z + e);

    vec3 tangentX = normalize(vec3(e, h_right - h, 0.0));
    vec3 tangentZ = normalize(vec3(0.0, h_back - h, e));

    Normal = normalize(cross(tangentZ, tangentX));

    Height = h;
    FragPos = vec3(worldPos);
    gl_Position = projection * view * worldPos;
}
