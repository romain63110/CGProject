#ifndef VIEWER_H
#define VIEWER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "node.h"

class Viewer {
public:
    Viewer(int width = 640, int height = 480);

    void run();

    void on_key(int key, int action);
    void on_mouse_move(double xpos, double ypos);
    void on_mouse_button(int button, int action);

    Node* scene_root;
    Node* aircraft_node = nullptr;

private:
    GLFWwindow* win;

    static void key_callback_static(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursor_pos_callback_static(GLFWwindow* window, double xpos, double ypos);
    static void mouse_button_callback_static(GLFWwindow* window, int button, int action, int mods);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    bool mouse_captured_ = false;
    bool first_mouse_ = true;

    double last_x_ = 0.0;
    double last_y_ = 0.0;

    // FreeCam (optionnel)
    float yaw_ = -90.0f;
    float pitch_ = 0.0f;

    glm::vec3 camera_pos_ = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 camera_front_ = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 camera_up_ = glm::vec3(0.0f, 1.0f, 0.0f);

    enum class CameraMode { FreeCam, FollowAircraft };
    CameraMode camera_mode_ = CameraMode::FollowAircraft;

    // Aircraft state
    glm::vec3 aircraft_pos_ = glm::vec3(0.0f, 0.0f, -5.0f);
    float aircraft_speed_ = 6.0f;

    glm::quat aircraft_orient_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
};

#endif
