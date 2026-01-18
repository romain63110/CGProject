#include "camera_controller.h"
#include "aircraft.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void CameraController::toggleMode()
{
    if (mode == Mode::FreeCam) mode = Mode::FollowAircraft;
    else mode = Mode::FreeCam;
}

void CameraController::updateFreeCamKeys(void* glfwWindow, float dt, CameraState& cam)
{
    GLFWwindow* win = (GLFWwindow*)glfwWindow;

    float camSpeed = 3.0f;
    float move = camSpeed * dt;

    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
        cam.pos += move * cam.front;

    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
        cam.pos -= move * cam.front;

    glm::vec3 camRight = glm::normalize(glm::cross(cam.front, cam.up));

    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
        cam.pos -= camRight * move;

    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
        cam.pos += camRight * move;
}

void CameraController::updateFollowCam(const Aircraft& ac, CameraState& cam)
{
    glm::vec3 forward = ac.orientation * glm::vec3(0, 0, -1);
    glm::vec3 offset = -forward * 12.0f + glm::vec3(0.0f, 5.0f, 0.0f);

    cam.pos = ac.position + offset;
    cam.front = glm::normalize(ac.position - cam.pos);
}

void CameraController::onMouseMove(double xpos, double ypos, CameraState& cam)
{
    if (!mouse_captured) return;
    if (mode != Mode::FreeCam) return;

    if (first_mouse)
    {
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
    }

    double xoffset = xpos - last_x;
    double yoffset = last_y - ypos;
    last_x = xpos;
    last_y = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += (float)xoffset;
    pitch += (float)yoffset;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cam.front = glm::normalize(front);
}
