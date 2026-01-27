#pragma once

#include "shape.h"
#include "shader.h"
#include <glm/glm.hpp>

class AfterburnerFlame : public Shape
{
public:
    AfterburnerFlame(Shader* shader, int slices = 24);
    ~AfterburnerFlame();

    float intensity = 0.0f;
    float timeSec = 0.0f;
    glm::vec3 color = glm::vec3(0.2f, 0.6f, 1.0f);

    void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection, glm::vec4& clipPlane) override;

private:
    unsigned int vao_ = 0;
    unsigned int vbo_ = 0;

    int vertexCount_ = 0;
    int slices_ = 24;

    void buildConeMesh();
};
