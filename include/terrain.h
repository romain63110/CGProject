#pragma once
#include "node.h" // Ta classe de base
#include "terrain_chunk.h"
#include "texture.h"
#include <map>
#include <string>
#include <vector>
#include <GL/glew.h>

class Terrain : public Node {
public:
    Terrain(Shader* terrainShader);
    ~Terrain(); // Important pour nettoyer le VAO partagé

    void update(glm::vec3 playerPos);

    // Override de Node
    void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection) override;

    float getGlobalHeight(float worldX, float worldZ);

private:
    std::map<std::string, TerrainChunk*> chunks_;
    GLuint shader_;

    // Paramètres globaux
    float chunkSize_ = 256.0f;
    int resolution_ = 128;     // divisions: (n x n)x2 triangles
    int renderDistance_ = 3; //nb of chunk

    // Shared between each chunk
    GLuint sharedVAO, sharedVBO, sharedEBO;
    int indexCount_;

    void generateSharedMesh();
    std::string getKey(int x, int z);

    // TExture 
    Texture* sandTex_;
    Texture* grassTex_;
    Texture* rockTex_;
    Texture* snowTex_;
};