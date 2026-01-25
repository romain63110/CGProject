#pragma once
#include "shape.h"
#include "shader.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>  // pour GLuint

class Cloud : public Shape {
public:
    Cloud(const std::string& path, Shader* shader);
    virtual ~Cloud() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection);

private:
    void setupMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

    //GLuint
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
};
