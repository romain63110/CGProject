#pragma once
#include "shape.h"
#include "texture.h"

class Skybox : public Shape {
public:
    Skybox(Shader* shader, Texture* texture);
    ~Skybox();
    void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection) override;

private:
    unsigned int VAO, VBO;
    Texture* texture_;
};