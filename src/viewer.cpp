#include "viewer.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "afterburner_flame.h"


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

        if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) controls_.aileron -= 1.0f;
        if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) controls_.aileron += 1.0f;

        if (glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS) controls_.throttle += 0.6f * dt;
        if (glfwGetKey(win, GLFW_KEY_F) == GLFW_PRESS) controls_.throttle -= 0.6f * dt;

        if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) controls_.elevator += 1.0f;
        if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) controls_.elevator -= 1.0f;

        if (controls_.throttle < 0.0f) controls_.throttle = 0.0f;
        if (controls_.throttle > 1.0f) controls_.throttle = 1.0f;

        flight_model_.step(aircraft_, controls_, dt);
        aircraft_.syncNode();

        if (camera_ctrl_.mode == CameraController::Mode::FreeCam)
            camera_ctrl_.updateFreeCamKeys(win, dt, camera_);
        else
            camera_ctrl_.updateFollowCam(aircraft_, dt, camera_);


        float t = (float)glfwGetTime();

        if (afterburnerL_)
        {
            afterburnerL_->timeSec = t;
            afterburnerL_->intensity = 1.0f;
        }

        if (afterburnerR_)
        {
            afterburnerR_->timeSec = t;
            afterburnerR_->intensity = 1.0f;
        }


        /*if (afterburner_)
        {
            afterburner_->timeSec = (float)glfwGetTime();

            float ab = 0.0f;
            if (controls_.throttle > 0.85f)
                ab = (controls_.throttle - 0.85f) / 0.15f;

            afterburner_->intensity = ab;
        }
        */

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Debug");

        ImGui::Text("FPS: %.1f", (dt > 0.0f ? 1.0f / dt : 0.0f));
        ImGui::Separator();

        ImGui::Text("Throttle: %.2f", controls_.throttle);
        ImGui::Text("Speed: %.2f km/h", flight_model_.last_speed * 3.6f);
        ImGui::Text("Mach: %.2f", (flight_model_.last_speed * 3.6f) / 1200.0f);

        ImGui::Text("AoA: %.2f deg", flight_model_.alpha_deg);
        ImGui::Text("CL: %.3f", flight_model_.last_CL);
        ImGui::Text("Lift: %.1f N", flight_model_.last_L);
        ImGui::SliderFloat("LiftPower", &flight_model_.liftPower, 0.0f, 10.0f);


        ImGui::Separator();

        ImGui::Text("V world : (%.2f, %.2f, %.2f)",
            flight_model_.dbg_v_world.x,
            flight_model_.dbg_v_world.y,
            flight_model_.dbg_v_world.z);

        ImGui::Text("V local : (%.2f, %.2f, %.2f)",
            flight_model_.dbg_v_local.x,
            flight_model_.dbg_v_local.y,
            flight_model_.dbg_v_local.z);

        ImGui::Separator();

        ImGui::Text("Ft: (%.1f, %.1f, %.1f)",
            flight_model_.dbg_Ft.x, flight_model_.dbg_Ft.y, flight_model_.dbg_Ft.z);

        ImGui::Text("Fd: (%.1f, %.1f, %.1f)",
            flight_model_.dbg_Fd.x, flight_model_.dbg_Fd.y, flight_model_.dbg_Fd.z);

        ImGui::Text("FL: (%.1f, %.1f, %.1f)",
            flight_model_.dbg_FL.x, flight_model_.dbg_FL.y, flight_model_.dbg_FL.z);

        ImGui::Text("Fg: (%.1f, %.1f, %.1f)",
            flight_model_.dbg_Fg.x, flight_model_.dbg_Fg.y, flight_model_.dbg_Fg.z);

        ImGui::Text("SumF: (%.1f, %.1f, %.1f)",
            flight_model_.dbg_Fsum.x, flight_model_.dbg_Fsum.y, flight_model_.dbg_Fsum.z);

        ImGui::Separator();

        ImGui::SliderFloat("Tmax", &flight_model_.Tmax, 0.0f, 40000.0f);
        ImGui::SliderFloat("WingArea", &flight_model_.wingArea, 1.0f, 60.0f);
        ImGui::SliderFloat("AirDensity rho", &flight_model_.rho, 0.2f, 2.0f);

        ImGui::Separator();
        ImGui::Text("Directional Drag");

        ImGui::SliderFloat("dragForward", &flight_model_.dragForward, 0.0f, 5.0f);
        ImGui::SliderFloat("dragBack", &flight_model_.dragBack, 0.0f, 10.0f);
        ImGui::SliderFloat("dragRight", &flight_model_.dragRight, 0.0f, 10.0f);
        ImGui::SliderFloat("dragLeft", &flight_model_.dragLeft, 0.0f, 10.0f);
        ImGui::SliderFloat("dragUp", &flight_model_.dragUp, 0.0f, 10.0f);
        ImGui::SliderFloat("dragDown", &flight_model_.dragDown, 0.0f, 10.0f);

        ImGui::End();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int width, height;
        glfwGetFramebufferSize(win, &width, &height);
        if (height == 0) height = 1;

        float aspect_ratio = (float)width / (float)height;

        glm::mat4 projection = glm::perspective(glm::radians(camera_.fov), aspect_ratio, 0.1f, 5000.0f);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(camera_.pos, camera_.pos + camera_.front, camera_.up);

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
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse) return;

        camera_ctrl_.mouse_captured = true;
        camera_ctrl_.first_mouse = true;
        glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void Viewer::on_mouse_move(double xpos, double ypos)
{
    camera_ctrl_.onMouseMove(xpos, ypos, camera_);
}
