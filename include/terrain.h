#pragma once

#include "node.h"
#include "terrain_chunk.h"
#include "texture.h"

#include <map>
#include <string>
#include <vector>

#include <GL/glew.h>

class Shader;

class Terrain : public Node {
public:
    Terrain(Shader* terrainShader);
    ~Terrain();

    void update(glm::vec3 playerPos);

    void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection, glm::vec4& clipPlane);

    float getGlobalHeight(float worldX, float worldZ);

private:
    std::map<std::string, TerrainChunk*> chunks_;
    GLuint shader_ = 0;

    float chunkSize_ = 256.0f;
    int resolution_ = 16;
    int renderDistance_ = 10;

    GLuint sharedVAO = 0;
    GLuint sharedVBO = 0;
    GLuint sharedEBO = 0;
    GLuint indexCount_ = 0;

    void generateSharedMesh();
    std::string getKey(int x, int z);

    Texture* sandTex_ = nullptr;
    Texture* grassTex_ = nullptr;
    Texture* rockTex_ = nullptr;
    Texture* snowTex_ = nullptr;
};
