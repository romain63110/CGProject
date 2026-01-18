#include "aircraft.h"
#include "node.h"
#include <glm/gtc/matrix_transform.hpp>

void Aircraft::syncNode()
{
    if (!node) return;

    glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 R = glm::mat4_cast(orientation);
    node->set_transform(T * R);
}
