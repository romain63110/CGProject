#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "mesh.h"

// Une instance = une position + rotation
struct CloudInstance {
    glm::vec3 position;
    float rotation;
};

// Données partagées UFO / Clouds
struct CloudData {
    Mesh mesh;
    std::vector<CloudInstance> instances;

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    GLuint textureID = 0;
};

// Charge un modèle + crée N instances aléatoires
CloudData initCloudField(const std::string& objPath, int count);
