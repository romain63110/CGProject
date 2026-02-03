#pragma once

#include <glm/glm.hpp>

struct CameraState
{
    glm::vec3 pos = glm::vec3(0, 0, 3);
    glm::vec3 front = glm::vec3(0, 0, -1);
    glm::vec3 up = glm::vec3(0, 1, 0);

    float fov = 45.0f;
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

    float currentFov = 45.0f;
    float fovBase = 45.0f;
    float fovMax = 62.0f;

    float fovSpeedMin = 30.0f;
    float fovSpeedMax = 140.0f;

    float fovSmooth = 6.0f;

    void toggleMode();
    void updateFreeCamKeys(void* glfwWindow, float dt, CameraState& cam);
    void updateFollowCam(const Aircraft& ac, float dt, CameraState& cam);
    void onMouseMove(double xpos, double ypos, CameraState& cam);
};
