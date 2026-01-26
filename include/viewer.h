#ifndef VIEWER_H
#define VIEWER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "node.h"

#include "aircraft.h"
#include "aircraft_controls.h"
#include "flight_model_physics.h"
#include "camera_controller.h"

class AfterburnerFlame;

class Viewer {
public:
    Viewer(int width = 640, int height = 480);

    void run();

    AfterburnerFlame* afterburnerL_ = nullptr;
    AfterburnerFlame* afterburnerR_ = nullptr;


    void on_key(int key, int action);
    void on_mouse_move(double xpos, double ypos);
    void on_mouse_button(int button, int action);

    Node* scene_root;

    Aircraft aircraft_;

private:
    GLFWwindow* win;

    static void key_callback_static(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursor_pos_callback_static(GLFWwindow* window, double xpos, double ypos);
    static void mouse_button_callback_static(GLFWwindow* window, int button, int action, int mods);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    AircraftControls controls_;
    FlightModelPhysics flight_model_;

    CameraState camera_;
    CameraController camera_ctrl_;
    double last_x_ = 0.0;
    double last_y_ = 0.0;

    //Euler
    float yaw_ = -90.0f;
    float pitch_ = 0.0f;

    // camera
    glm::vec3 camera_pos_ = glm::vec3(0.0f, 20.0f, 3.0f);
    glm::vec3 camera_front_ = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 camera_up_ = glm::vec3(0.0f, 1.0f, 0.0f);
};

#endif
