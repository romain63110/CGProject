#pragma once

#include <glm/glm.hpp>

class TerrainChunk {
public:
    // Constructeur simplifié : plus besoin de shader ou de résolution pour le mesh
    TerrainChunk(int xIndex, int zIndex, float size, float heightScale);

    ~TerrainChunk();

    // Fonction cruciale pour la physique :
    // Elle recalcule mathématiquement le Perlin Noise pour une position donnée.
    // Simule ce que fait le Vertex Shader.
    float getHeight(float localX, float localZ);

    // Getters utilisés par la classe Terrain pour positionner le mesh partagé
    int getXIndex() const { return xIndex_; }
    int getZIndex() const { return zIndex_; }

private:
    int xIndex_;        // Index en X dans la grille infinie (ex: 0, 1, -1...)
    int zIndex_;        // Index en Z dans la grille infinie
    float size_;        // Taille réelle du chunk (ex: 64.0f)
    float heightScale_; // Amplitude de la hauteur (doit matcher l'uniform du shader)
};