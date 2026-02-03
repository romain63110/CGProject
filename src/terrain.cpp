#include "terrain.h"
#include <iostream>
#include <vector>

Terrain::Terrain(Shader* terrainShader) : Node(glm::mat4(1.0f)), shader_(terrainShader->get_id()) {
    std::cout << "Terrain created" << std::endl;

    // Texture Loading
    try {
        sandTex_ = new Texture("../../../textures/sand.jpg", GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
        grassTex_ = new Texture("../../../textures/grass.jpg", GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
        rockTex_ = new Texture("../../../textures/rock.jpg", GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
        snowTex_ = new Texture("../../../textures/snow.jpg", GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur de chargement textures terrain: " << e.what() << std::endl;
    }

    glUseProgram(shader_);
    glUniform1i(glGetUniformLocation(shader_, "textureSand"), 0);
    glUniform1i(glGetUniformLocation(shader_, "textureGrass"), 1);
    glUniform1i(glGetUniformLocation(shader_, "textureRock"), 2);
    glUniform1i(glGetUniformLocation(shader_, "textureSnow"), 3);

    // Mesh Loading
    generateSharedMesh();
}

Terrain::~Terrain() {
    glDeleteVertexArrays(1, &sharedVAO);
    glDeleteBuffers(1, &sharedVBO);
    glDeleteBuffers(1, &sharedEBO);

    // Optionnel mais conseillé : libérer les chunks
    for (auto it = chunks_.begin(); it != chunks_.end(); ++it) {
        delete it->second;
    }
    chunks_.clear();
}

void Terrain::generateSharedMesh() {
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    float step = chunkSize_ / (float)resolution_;

    // generate shared flat plane (0,0) -> (chunkSize, chunkSize)
    for (int z = 0; z <= resolution_; ++z) {
        for (int x = 0; x <= resolution_; ++x) {
            vertices.push_back(x * step); // X
            vertices.push_back(0.0f);     // Y (flatten)
            vertices.push_back(z * step); // Z
        }
    }

    // EBO
    for (int z = 0; z < resolution_; ++z) {
        for (int x = 0; x < resolution_; ++x) {
            int current = z * (resolution_ + 1) + x;
            int nextRow = (z + 1) * (resolution_ + 1) + x;

            indices.push_back(current);
            indices.push_back(nextRow);
            indices.push_back(current + 1);

            indices.push_back(current + 1);
            indices.push_back(nextRow);
            indices.push_back(nextRow + 1);
        }
    }

    indexCount_ = (GLuint)indices.size();

    // OpenGL Setup
    glGenVertexArrays(1, &sharedVAO);
    glBindVertexArray(sharedVAO);

    glGenBuffers(1, &sharedVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sharedVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &sharedEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sharedEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Terrain::update(glm::vec3 playerPos) {
    int currentChunkX = (int)floor(playerPos.x / chunkSize_);
    int currentChunkZ = (int)floor(playerPos.z / chunkSize_);

    std::vector<std::string> activeKeys;

    for (int x = -renderDistance_; x <= renderDistance_; x++) {
        for (int z = -renderDistance_; z <= renderDistance_; z++) {
            int targetX = currentChunkX + x;
            int targetZ = currentChunkZ + z;
            std::string key = getKey(targetX, targetZ);

            activeKeys.push_back(key);

            if (chunks_.find(key) == chunks_.end()) { // create new if it doesn't exist
                TerrainChunk* newChunk = new TerrainChunk(targetX, targetZ, chunkSize_, 4.0f);
                chunks_[key] = newChunk;
                std::cout << "Chunk created: " << key << std::endl;
            }
        }
    }

    // delete distant chunks
    auto it = chunks_.begin();
    while (it != chunks_.end()) {

        bool keep = false;
        for (const auto& k : activeKeys) {
            if (k == it->first) {
                keep = true;
                break;
            }
        }

        if (!keep) {
            delete it->second;
            it = chunks_.erase(it);
        }
        else {
            ++it;
        }
    }
}

void Terrain::draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection) {
    glUseProgram(shader_);

    if (sandTex_)  sandTex_->bind(0);
    if (grassTex_) grassTex_->bind(1);
    if (rockTex_)  rockTex_->bind(2);
    if (snowTex_)  snowTex_->bind(3);

    glUniformMatrix4fv(glGetUniformLocation(shader_, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader_, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader_, "projection"), 1, GL_FALSE, &projection[0][0]);

    glUniform1f(glGetUniformLocation(shader_, "heightScale"), 40.0f);

    glBindVertexArray(sharedVAO); // Loaded one time for all chunk

    // ? Compatible C++11/C++14 (pas de structured bindings)
    for (auto it = chunks_.begin(); it != chunks_.end(); ++it) {
        // const std::string& key = it->first; // pas utilisé, mais dispo si besoin
        TerrainChunk* chunk = it->second;

        // We calculate the position in the world for each chunk
        float xPos = chunk->getXIndex() * chunkSize_;
        float zPos = chunk->getZIndex() * chunkSize_;

        glm::mat4 chunkModel = glm::translate(glm::mat4(1.0f), glm::vec3(xPos, 0, zPos));
        glUniformMatrix4fv(glGetUniformLocation(shader_, "model"), 1, GL_FALSE, &chunkModel[0][0]);

        glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
}

float Terrain::getGlobalHeight(float worldX, float worldZ) {
    int chunkX = (int)floor(worldX / chunkSize_);
    int chunkZ = (int)floor(worldZ / chunkSize_);

    std::string key = getKey(chunkX, chunkZ);

    if (chunks_.find(key) != chunks_.end()) {
        float localX = worldX - (chunkX * chunkSize_);
        float localZ = worldZ - (chunkZ * chunkSize_);

        return chunks_[key]->getHeight(localX, localZ);
    }

    return -100.0f; // default value if there is nothing
}

std::string Terrain::getKey(int x, int z) {
    return std::to_string(x) + "_" + std::to_string(z);
}
