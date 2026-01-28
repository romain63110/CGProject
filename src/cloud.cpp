#include "cloud.h"

#include <cstdlib>
#include <ctime>
#include <glm/gtc/type_ptr.hpp>

extern Mesh loadOBJ(const std::string& path);
extern unsigned int loadTexture(const char* path);

CloudData initCloudField(const std::string& objPath, int count)
{
    CloudData data;
    data.mesh = loadOBJ(objPath);

    // --- Instances ---
    std::srand((unsigned int)std::time(nullptr));

    for (int i = 0; i < count; ++i) {
        CloudInstance inst;

        inst.position.x = (std::rand() / float(RAND_MAX) - 0.5f) * 10000.0f;
        inst.position.z = (std::rand() / float(RAND_MAX) - 0.5f) * 10000.0f;

  
        inst.position.y = 100.0f +
            (std::rand() / float(RAND_MAX)) * 400.0f;

        inst.rotation = float(std::rand() % 360);

        data.instances.push_back(inst);
    }

    // --- OpenGL buffers ---
    glGenVertexArrays(1, &data.VAO);
    glGenBuffers(1, &data.VBO);
    glGenBuffers(1, &data.EBO);

    glBindVertexArray(data.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, data.VBO);
    glBufferData(GL_ARRAY_BUFFER,
        data.mesh.vertices.size() * sizeof(Vertex),
        data.mesh.vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        data.mesh.indices.size() * sizeof(unsigned int),
        data.mesh.indices.data(),
        GL_STATIC_DRAW);

    // layout: position / normal / uv
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);

    // Texture (à adapter au chemin réel)
    data.textureID = loadTexture("textures/ufo.jpg");

    return data;
}
