#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Node;

struct Aircraft
{
    Node* node = nullptr;

    glm::vec3 position = glm::vec3(0, 0, -5);
    glm::vec3 velocity = glm::vec3(0, 0, 0);

    glm::quat orientation = glm::quat(1, 0, 0, 0);
    glm::vec3 omega_body = glm::vec3(0, 0, 0);

    void syncNode();
};
