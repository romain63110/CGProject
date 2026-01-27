#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <vector>
#include<string>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords; 
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
};

Mesh loadOBJ(const std::string& path);
unsigned int loadTexture(char const * path);

#endif