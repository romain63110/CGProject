#include "viewer.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

Viewer::Viewer(int width, int height)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        glfwTerminate();
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    win = glfwCreateWindow(width, height, "Viewer", NULL, NULL);

    if (win == NULL) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(win);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
    }

    glfwSwapInterval(1);

    glfwSetWindowUserPointer(win, this);

    glfwSetKeyCallback(win, key_callback_static);
    glfwSetCursorPosCallback(win, cursor_pos_callback_static);
    glfwSetMouseButtonCallback(win, mouse_button_callback_static);
    glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);

    last_x_ = width / 2.0;
    last_y_ = height / 2.0;

    std::cout << glGetString(GL_VERSION) << ", GLSL "
        << glGetString(GL_SHADING_LANGUAGE_VERSION) << ", Renderer "
        << glGetString(GL_RENDERER) << std::endl;

    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    scene_root = new Node();
}

void Viewer::run()
{
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(win))
    {
        double now = glfwGetTime();
        float dt = float(now - lastTime);
        lastTime = now;

        glfwPollEvents();

        // ======================
        // Aircraft axes (local)
        // ======================
        glm::vec3 forward = aircraft_orient_ * glm::vec3(0, 0, -1);
        glm::vec3 right = aircraft_orient_ * glm::vec3(1, 0, 0);
        glm::vec3 up = aircraft_orient_ * glm::vec3(0, 1, 0);

        // ======================
        // Inputs
        // ======================
        float pitchInput = 0.0f;
        float rollInput = 0.0f;

   
        if (glfwGetKey(win, GLFW_KEY_Z) == GLFW_PRESS) pitchInput += 1.0f;
        if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) pitchInput -= 1.0f;

        if (glfwGetKey(win, GLFW_KEY_Q) == GLFW_PRESS) rollInput -= 1.0f;
        if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) rollInput += 1.0f;

        // Mouse -> pitch + yaw (local)
        float mousePitchInput = -mouse_dy_; // souris haut -> pitch up
        float mouseYawInput = mouse_dx_; // souris droite -> yaw right

        mouse_dx_ = 0.0f;
        mouse_dy_ = 0.0f;

        // ======================
        // Apply rotations locally (Quaternion)
        // ======================
        float pitch_rate = glm::radians(80.0f);   // rad/s
        float roll_rate = glm::radians(140.0f);  // rad/s
        float yaw_rate = glm::radians(90.0f);   // rad/s

        // Pitch : around aircraft RIGHT axis
        glm::quat qPitch = glm::angleAxis(pitch_rate * pitchInput * dt, right);

        // Roll : around aircraft FORWARD axis
        glm::quat qRoll = glm::angleAxis(roll_rate * rollInput * dt, forward);

        // Yaw : around aircraft UP axis (mouse)
        glm::quat qYaw = glm::angleAxis(yaw_rate * mouseYawInput * dt, up);

        // Mouse pitch : around aircraft RIGHT axis
        glm::quat qMousePitch = glm::angleAxis(pitch_rate * mousePitchInput * dt, right);

        // Combine rotations
        aircraft_orient_ = glm::normalize(qYaw * qRoll * qMousePitch * qPitch * aircraft_orient_);

        // Recompute forward after rotation
        forward = aircraft_orient_ * glm::vec3(0, 0, -1);

        // ======================
        // Move forward
        // ======================
        aircraft_pos_ += forward * aircraft_speed_ * dt;

        // ======================
        // Apply transform to aircraft node
        // ======================
        if (aircraft_node)
        {
            glm::mat4 T = glm::translate(glm::mat4(1.0f), aircraft_pos_);
            glm::mat4 R = glm::mat4_cast(aircraft_orient_);
            aircraft_node->set_transform(T * R);
        }

        // ======================
        // Camera update
        // ======================
        if (camera_mode_ == CameraMode::FreeCam)
        {
            float camSpeed = 3.0f;
            float move = camSpeed * dt;

            if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
                camera_pos_ += move * camera_front_;

            if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
                camera_pos_ -= move * camera_front_;

            glm::vec3 camRight = glm::normalize(glm::cross(camera_front_, camera_up_));

            if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
                camera_pos_ -= camRight * move;

            if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
                camera_pos_ += camRight * move;
        }
        else
        {
            // FollowCam behind aircraft
            glm::vec3 camOffset = -forward * 10.0f + glm::vec3(0.0f, 3.5f, 0.0f);
            camera_pos_ = aircraft_pos_ + camOffset;
            camera_front_ = glm::normalize(aircraft_pos_ - camera_pos_);
        }

        // ======================
        // Render
        // ======================
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int width, height;
        glfwGetFramebufferSize(win, &width, &height);
        if (height == 0) height = 1;

        float aspect_ratio = (float)width / (float)height;

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 200.0f);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(camera_pos_, camera_pos_ + camera_front_, camera_up_);

        scene_root->draw(model, view, projection);

        glfwSwapBuffers(win);
    }

    glfwTerminate();
}

void Viewer::key_callback_static(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Viewer* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));
    viewer->on_key(key, action);
}

void Viewer::cursor_pos_callback_static(GLFWwindow* window, double xpos, double ypos)
{
    Viewer* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));
    viewer->on_mouse_move(xpos, ypos);
}

void Viewer::mouse_button_callback_static(GLFWwindow* window, int button, int action, int mods)
{
    Viewer* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));
    viewer->on_mouse_button(button, action);
}

void Viewer::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Viewer::on_key(int key, int action)
{
    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_C)
    {
        if (camera_mode_ == CameraMode::FreeCam)
            camera_mode_ = CameraMode::FollowAircraft;
        else
            camera_mode_ = CameraMode::FreeCam;
    }

    if (key == GLFW_KEY_ESCAPE)
    {
        if (mouse_captured_) {
            mouse_captured_ = false;
            glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
            glfwSetWindowShouldClose(win, GLFW_TRUE);
        }
    }
}

void Viewer::on_mouse_button(int button, int action)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (!mouse_captured_) {
            mouse_captured_ = true;
            first_mouse_ = true;
            glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}

void Viewer::on_mouse_move(double xpos, double ypos)
{
    if (!mouse_captured_) return;

    if (first_mouse_)
    {
        last_x_ = xpos;
        last_y_ = ypos;
        first_mouse_ = false;
    }

    double xoffset = xpos - last_x_;
    double yoffset = last_y_ - ypos;
    last_x_ = xpos;
    last_y_ = ypos;

    float sensitivity = 0.03f; // plus petit car on est en quaternions
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // store mouse deltas (applied in run)
    mouse_dx_ += (float)xoffset;
    mouse_dy_ += (float)yoffset;

    // update freecam direction if needed
    if (camera_mode_ == CameraMode::FreeCam)
    {
        yaw_ += (float)(xoffset * 100.0f);
        pitch_ += (float)(yoffset * 100.0f);

        if (pitch_ > 89.0f)  pitch_ = 89.0f;
        if (pitch_ < -89.0f) pitch_ = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front.y = sin(glm::radians(pitch_));
        front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        camera_front_ = glm::normalize(front);
    }
}
