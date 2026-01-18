#pragma once
#include "shape.h"
#include <string>
#include <vector>

class ObjModel : public Shape
{
public:
    ObjModel(Shader* shader_program, const std::string& objPath);
    ~ObjModel();

    void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection) override;

private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    int vertexCount = 0;
};
