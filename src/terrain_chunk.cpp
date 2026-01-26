#include "terrain_chunk.h"
#include <iostream>
#include <glm/gtc/noise.hpp>

TerrainChunk::TerrainChunk(int xIndex, int zIndex, float size, float heightScale)
    : xIndex_(xIndex), zIndex_(zIndex), size_(size), heightScale_(heightScale) {}

TerrainChunk::~TerrainChunk() {}

float TerrainChunk::getHeight(float localX, float localZ) {
    // Global coord
    float globalX = (xIndex_ * size_) + localX;
    float globalZ = (zIndex_ * size_) + localZ;

    // Parameters (/!\ must be the same on the shader)
    float zoneScale = 0.005f;
    float zoneAmplitude = 200.0f;

    float detailScale = 0.03f;
    float detailAmplitude = 3.0f;

    //MACRO: zone
    float macroNoise = glm::simplex(glm::vec2(globalX * zoneScale, globalZ * zoneScale));

    //MICRO: details
    float detailNoise = glm::simplex(glm::vec2(globalX * detailScale, globalZ * detailScale));


    // Base Heght
    float baseHeight = macroNoise * zoneAmplitude;

    // Mountain rougher than the earth
    float roughness = glm::smoothstep(-0.5f, 0.5f, macroNoise);

    float finalH = baseHeight + (detailNoise * detailAmplitude * roughness);

    // flat bottom ocean
    if (finalH < -5.0f) {
        finalH = -5.0f;
    }

    return finalH;
}