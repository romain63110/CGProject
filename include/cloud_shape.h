#pragma once

#include "shape.h"
#include "cloud.h"
#include "shader.h"

class CloudShape : public Shape {
public:

    CloudShape(Shader* shader,
        CloudData* data,
        bool animated = false);

    void draw(glm::mat4& model,
        glm::mat4& view,
        glm::mat4& projection,
        glm::vec4& clipPlane) override;

private:
    CloudData* data_;
    bool animated_;
};
