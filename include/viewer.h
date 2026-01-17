#ifndef VIEWER_H
#define VIEWER_H

#include <vector>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "shader.h"
#include "node.h"
#include "aircraft_sim.h"

class Viewer {
public:
    Viewer(int width = 640, int height = 480, AircraftSim* sim);

    void run();
    void on_key(int key);
    void on_mouse_move(double xpos, double ypos);
    void on_mouse_button(int button, int action);

    void setAircraftNode(Node* node);

    Node* scene_root;

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

    float yaw_ = -90.0f;
    float pitch_ = 0.0f;

    glm::vec3 camera_pos_ = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 camera_front_ = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 camera_up_ = glm::vec3(0.0f, 1.0f, 0.0f);

    AircraftSim* sim_ = nullptr;
    Node* aircraft_node_ = nullptr;
};

#endif
