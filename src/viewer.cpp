#include "viewer.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "afterburner_flame.h"
#include "terrain.h"

// ------------------------------------------------------------

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

    win = glfwCreateWindow(width, height, "Viewer", nullptr, nullptr);
    if (!win)
    {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(win);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
    }

    glfwSwapInterval(0); // VSync OFF

    glfwSetWindowUserPointer(win, this);
    glfwSetKeyCallback(win, key_callback_static);
    glfwSetCursorPosCallback(win, cursor_pos_callback_static);
    glfwSetMouseButtonCallback(win, mouse_button_callback_static);
    glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    scene_root = new Node();
}

// ------------------------------------------------------------

void Viewer::run()
{
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(win))
    {
        double now = glfwGetTime();
        float dt = float(now - lastTime);
        lastTime = now;

        glfwPollEvents();
        controls_.resetAxis();

        // -------- Controls --------
        if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) controls_.aileron -= 1.0f;
        if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) controls_.aileron += 1.0f;

        if (glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS) controls_.throttle += 0.6f * dt;
        if (glfwGetKey(win, GLFW_KEY_F) == GLFW_PRESS) controls_.throttle -= 0.6f * dt;

        if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) controls_.elevator += 0.6f;
        if (glfwGetKey(win, GLFW_KEY_X) == GLFW_PRESS) controls_.elevator += 0.2f;
        if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) controls_.elevator -= 0.2f;

        controls_.throttle = glm::clamp(controls_.throttle, 0.0f, 1.0f);

        flight_model_.step(aircraft_, controls_, dt);
        aircraft_.syncNode();

        // -------- Terrain --------
        if (terrain_ && aircraft_.node)
        {
            glm::vec3 planePos =
                glm::vec3(aircraft_.node->get_transform()[3]);
            terrain_->update(planePos);
        }

        // -------- Camera --------
        if (camera_ctrl_.mode == CameraController::Mode::FreeCam)
            camera_ctrl_.updateFreeCamKeys(win, dt, camera_);
        else
            camera_ctrl_.updateFollowCam(aircraft_, dt, camera_);

        // -------- Afterburner --------
        float t = (float)glfwGetTime();
        float ab = (controls_.throttle > 0.9f)
            ? (controls_.throttle - 0.9f) / 0.1f
            : 0.0f;

        if (afterburnerL_) { afterburnerL_->timeSec = t; afterburnerL_->intensity = ab; }
        if (afterburnerR_) { afterburnerR_->timeSec = t; afterburnerR_->intensity = ab; }

        // -------- ImGui --------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Debug");
        ImGui::Text("FPS: %.1f", dt > 0.f ? 1.f / dt : 0.f);
        ImGui::Text("Throttle: %.2f", controls_.throttle);
        ImGui::Text("Speed: %.2f km/h", flight_model_.last_speed * 3.6f);
        ImGui::Text("Mach: %.2f", (flight_model_.last_speed * 3.6f) / 1200.0f);
        ImGui::Text("UFO: %s", ufo_node_ ? "ON" : "OFF");
        ImGui::End();

        // -------- Render --------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int width, height;
        glfwGetFramebufferSize(win, &width, &height);
        if (height == 0) height = 1;

        float aspect = float(width) / float(height);

        glm::mat4 projection =
            glm::perspective(glm::radians(camera_.fov), aspect, 0.1f, 5000.0f);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view =
            glm::lookAt(camera_.pos, camera_.pos + camera_.front, camera_.up);

        // =========================
        // ?? UFO animation
        // =========================
        if (ufo_node_)
        {
            float hoverY = sin(t * 1.5f) * 3.0f;
            float rotation = t * 0.8f;

            glm::mat4 tr = glm::translate(glm::mat4(1.0f),
                glm::vec3(0.0f, 120.0f + hoverY, 0.0f));
            tr = glm::rotate(tr, rotation, glm::vec3(0, 1, 0));
            tr = glm::scale(tr, glm::vec3(10.0f));

            ufo_node_->set_transform(tr);
        }

        scene_root->draw(model, view, projection);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(win);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

// ------------------------------------------------------------
// Callbacks
// ------------------------------------------------------------

void Viewer::key_callback_static(GLFWwindow* window, int key, int, int action, int)
{
    static_cast<Viewer*>(glfwGetWindowUserPointer(window))->on_key(key, action);
}

void Viewer::cursor_pos_callback_static(GLFWwindow* window, double xpos, double ypos)
{
    static_cast<Viewer*>(glfwGetWindowUserPointer(window))->on_mouse_move(xpos, ypos);
}

void Viewer::mouse_button_callback_static(GLFWwindow* window, int button, int action, int)
{
    static_cast<Viewer*>(glfwGetWindowUserPointer(window))->on_mouse_button(button, action);
}

void Viewer::framebuffer_size_callback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Viewer::on_key(int key, int action)
{
    if (action != GLFW_PRESS) return;

    if (key == GLFW_KEY_C)
        camera_ctrl_.toggleMode();

    if (key == GLFW_KEY_TAB)
    {
        camera_ctrl_.mouse_captured = !camera_ctrl_.mouse_captured;
        camera_ctrl_.first_mouse = true;
        glfwSetInputMode(win, GLFW_CURSOR,
            camera_ctrl_.mouse_captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

    if (key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(win, GLFW_TRUE);
}

void Viewer::on_mouse_button(int button, int action)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (ImGui::GetIO().WantCaptureMouse) return;
        camera_ctrl_.mouse_captured = true;
        camera_ctrl_.first_mouse = true;
        glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void Viewer::on_mouse_move(double xpos, double ypos)
{
    camera_ctrl_.onMouseMove(xpos, ypos, camera_);
}
