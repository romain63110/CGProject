#ifndef CLOUD_H
#define CLOUD_H

#include <GL/glew.h>
#include "../include/mesh.h"
#include <string>

struct CloudInstance {
    glm::vec3 position;
    float rotation;
};

struct CloudData {
    Mesh mesh;
    GLuint VAO, VBO, EBO;
    unsigned int textureID;
    std::vector<CloudInstance> instances; // stores all cloud positions here
};

CloudData initCloudField(const std::string& path, int count);

#endif