#pragma once

#include <glm/glm.hpp>

struct CameraState
{
    glm::vec3 pos = glm::vec3(0, 0, 3);
    glm::vec3 front = glm::vec3(0, 0, -1);
    glm::vec3 up = glm::vec3(0, 1, 0);
};

struct Aircraft;

class CameraController
{
public:
    enum class Mode { FreeCam, FollowAircraft };
    Mode mode = Mode::FollowAircraft;

    bool mouse_captured = false;
    bool first_mouse = true;
    double last_x = 0.0;
    double last_y = 0.0;

    float yaw = -90.0f;
    float pitch = 0.0f;

    void toggleMode();
    void updateFreeCamKeys(void* glfwWindow, float dt, CameraState& cam);
    void updateFollowCam(const Aircraft& ac, CameraState& cam);
    void onMouseMove(double xpos, double ypos, CameraState& cam);
};
