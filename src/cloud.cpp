#include "../include/cloud.h"
#include <iostream>
#include <ctime>


//loads the obj file once and does VBO/VAO for either clouds or ufo depending on the path
//it was preferable to not write nother ufo.cpp file because it will just be like this one
//plus added instance to generate random coordinates for the clouds  
CloudData initCloudField(const std::string& path, int count) {
    CloudData data = {};
    data.mesh = loadOBJ(path);
    
    glGenVertexArrays(1, &data.VAO);
    glBindVertexArray(data.VAO);

    glGenBuffers(1, &data.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, data.VBO);

    glBufferData(GL_ARRAY_BUFFER, data.mesh.vertices.size() * sizeof(Vertex), &data.mesh.vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &data.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.mesh.indices.size() * sizeof(unsigned int), &data.mesh.indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    if (count == 1) {
        data.textureID = loadTexture("../../../textures/ufo.jpg");
    } else {
        data.textureID = 0;
    }

    // Instance Logic
    for (int i = 0; i < count; i++) {
        CloudInstance inst;
        if (count ==1) { // its the UFO
            float x = 0; 
            float z = -1;                      
            float y = 0;                       
            inst.position = glm::vec3(x, y, z); 
        } else { // its clouds
            float x = (float)(rand()%1000)-600.0f;
            float z = (float)(rand()%1000)-600.0f;
            float y = (float)(rand()%80)-10.0f;
            inst.position = glm::vec3(x,y,z);
        }
        inst.rotation = (float)(rand() % 360);
        data.instances.push_back(inst);
    }

    return data;
}