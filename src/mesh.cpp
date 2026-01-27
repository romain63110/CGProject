
#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h" 
#include "../include/mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <fstream>   
#include <sstream>   
#include <string>

// for importing .obj object to opengl using assimp library
// handles quads and triangles (clouds and ufo)
// code adjusted by mathematically splitting the 4 points into 2 tirangles


Mesh loadOBJ(const std::string& path) {
    Mesh mesh;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    std::ifstream file(path);
    if (!file.is_open()) { return mesh; }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 v; ss >> v.x >> v.y >> v.z;
            temp_vertices.push_back(v);
        } else if (prefix == "vt") {
            glm::vec2 vt; ss >> vt.x >> vt.y;
            temp_uvs.push_back(vt);
        } else if (prefix == "vn") {
            glm::vec3 vn; ss >> vn.x >> vn.y >> vn.z;
            temp_normals.push_back(vn);
        } else if (prefix == "f") {
            std::vector<Vertex> faceVertices;
            std::string vData;
            while (ss >> vData) {
                int vIdx = -1, tIdx = -1, nIdx = -1;
                // Parse v/t/n format
                if (sscanf(vData.c_str(), "%d/%d/%d", &vIdx, &tIdx, &nIdx) == 3) {
                    Vertex v;
                    v.Position = temp_vertices[vIdx - 1];
                    v.TexCoords = temp_uvs[tIdx - 1];
                    v.Normal = temp_normals[nIdx - 1];
                    faceVertices.push_back(v);
                }
            }
            // Triangulate Quads: 0-1-2 and 0-2-3
            for (size_t i = 1; i < faceVertices.size() - 1; i++) {
                unsigned int startIdx = mesh.vertices.size();
                mesh.vertices.push_back(faceVertices[0]);
                mesh.vertices.push_back(faceVertices[i]);
                mesh.vertices.push_back(faceVertices[i+1]);
                mesh.indices.push_back(startIdx);
                mesh.indices.push_back(startIdx + 1);
                mesh.indices.push_back(startIdx + 2);
            }
        }
    }
    return mesh;
}

unsigned int loadTexture(char const * path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    // STB_image does the heavy lifting of opening the file
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    
    if (data) {
        GLenum format;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else if (nrComponents == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        // Upload the raw pixels to the GPU's memory
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set parameters so the texture repeats nicely
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout << "Texture loaded successfully: " << path << std::endl;
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}