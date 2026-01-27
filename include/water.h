#ifndef WATER_H
#define WATER_H

#include "node.h"
#include "water_framebuffers.h"

class Water : public Node {
public:
    Water(Shader* shader, WaterFrameBuffers* fbos);
    ~Water();
    void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection);

private:
    GLuint VAO, VBO;
    GLuint shader_;
    WaterFrameBuffers* fbos_;
};

#endif