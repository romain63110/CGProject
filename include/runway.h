#pragma once
#include "shape.h"

class Runway : public Shape
{
public:
    Runway(Shader* shader_program, float length, float width);
    ~Runway();

    void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection) override;

private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    int vertexCount = 0;
};
