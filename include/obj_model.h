#pragma once

#include "shape.h"
#include <string>
#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>
#include <GL/glew.h>

class ObjModel : public Shape
{
public:
    ObjModel(Shader* shader_program, const std::string& objPath);
    ~ObjModel();

    void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection, glm::vec4& clipPlane);

private:
    struct SubMesh
    {
        GLuint VAO = 0;
        GLuint VBO = 0;
        int vertexCount = 0;

        glm::vec3 Kd = glm::vec3(1.0f);
        glm::vec3 Ks = glm::vec3(0.0f);
        float Ns = 32.0f;

        std::string materialName;
    };

    std::vector<SubMesh> submeshes_;
};
