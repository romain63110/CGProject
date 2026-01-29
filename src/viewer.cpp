#include "viewer.h"
#include "cloud.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "afterburner_flame.h"
#include "terrain.h" 
#include "water.h"


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

    
    glfwSwapInterval(0);

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
    glEnable(GL_CLIP_PLANE0);

    waterFBOs = new WaterFrameBuffers(); // Create FBOs
    std::string shader_dir = SHADER_DIR;
    Shader* waterShader = new Shader(shader_dir + "water.vert", shader_dir + "water.frag");
    waterObject = new Water(waterShader, waterFBOs);

    scene_root = new Node();
}

void Viewer::renderScene(glm::mat4 view, glm::mat4 projection, glm::vec4 clipPlane) {
    glm::mat4 model = glm::mat4(1.0f);
    scene_root->draw(model, view, projection, clipPlane);
}

void Viewer::run()
{
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(win))
    {
        float waterHeight = 1.0f;

        double now = glfwGetTime();
        float dt = float(now - lastTime);
        lastTime = now;

        glfwPollEvents();

        controls_.resetAxis();

        if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) controls_.aileron -= 1.0f;
        if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) controls_.aileron += 1.0f;

        if (glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS) controls_.throttle += 0.6f * dt;
        if (glfwGetKey(win, GLFW_KEY_F) == GLFW_PRESS) controls_.throttle -= 0.6f * dt;

        if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) controls_.elevator += 0.6f;
        if (glfwGetKey(win, GLFW_KEY_X) == GLFW_PRESS) controls_.elevator += 0.2f;
        if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) controls_.elevator -= 0.2f;

        if (controls_.throttle < 0.0f) controls_.throttle = 0.0f;
        if (controls_.throttle > 1.0f) controls_.throttle = 1.0f;

        flight_model_.step(aircraft_, controls_, dt);
        aircraft_.syncNode();

        
        if (terrain_ && aircraft_.node)
        {
            glm::vec3 planePos = glm::vec3(aircraft_.node->get_transform()[3]);
            terrain_->update(planePos);
        }

        if (camera_ctrl_.mode == CameraController::Mode::FreeCam)
            camera_ctrl_.updateFreeCamKeys(win, dt, camera_);
        else
            camera_ctrl_.updateFollowCam(aircraft_, dt, camera_);

        float t = (float)glfwGetTime();

        float ab = 0.0f;
        if (controls_.throttle > 0.9f)
            ab = (controls_.throttle - 0.9f) / 0.1f;

        if (afterburnerL_)
        {
            afterburnerL_->timeSec = t;
            afterburnerL_->intensity = ab;
        }

        if (afterburnerR_)
        {
            afterburnerR_->timeSec = t;
            afterburnerR_->intensity = ab;
        }

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

        //REFRACTION ---
        waterFBOs->bindRefractionFrameBuffer();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(camera_.pos, camera_.pos + camera_.front, camera_.up);

        // ===== FOG TERRAIN =====
        if (terrain_)
        {
            terrain_->setCameraPos(camera_.pos); 

        }


        //Draw the scene using Clipping (cut everything that is ON the water: 0, -1, 0, waterHeight)
        renderScene(view, projection, glm::vec4(0, -1, 0, waterHeight));

        //REFLECTION ---
        waterFBOs->bindReflectionFrameBuffer();
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //We put the camera at the bottom
        float distance = 2 * (camera_.pos.y - waterHeight);
        glm::vec3 cameraReflectPos = camera_.pos;
        cameraReflectPos.y -= distance;

        // Reverse pitch 
        glm::vec3 camTarget = camera_.pos + camera_.front;
        glm::vec3 cameraReflectTarget = camTarget;
        cameraReflectTarget.y -= 2.0f * (camTarget.y - waterHeight);

        glm::mat4 viewReflect = glm::lookAt(cameraReflectPos, cameraReflectTarget, camera_.up);

        // Draw the scene using Clipping (cut everything that is UNDER water: 0, 1, 0, -waterHeight)
        // Ax + By + Cz + D = 0. Ici Y > waterHeight.
        renderScene(viewReflect, projection, glm::vec4(0, 1, 0, -waterHeight));

        // --- DÉBUT DEBUG EAU (Reflexion + Réfraction) ---

                
        GLuint texReflect = waterFBOs->getReflectionTexture();
        GLuint texRefract = waterFBOs->getRefractionTexture(); 

      
        float debugW = 300.0f;
        float debugH = 200.0f; 
        float padding = 10.0f;

        // Position: Coin Haut-Droit
        ImGui::SetNextWindowPos(ImVec2(width - debugW - padding, padding), ImGuiCond_Always);

        // On force la largeur, mais on laisse la hauteur automatique (0) pour qu'elle s'adapte au contenu
        ImGui::SetNextWindowSize(ImVec2(debugW + 20, 0), ImGuiCond_Always);

        // Flags: Pas de déplacement, redimensionnement auto
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;

        if (ImGui::Begin("Water Debug", nullptr, flags))
        {
            
            ImGui::Text("Reflection (Miroir)");
            ImGui::Image((void*)(intptr_t)texReflect,
                ImVec2(debugW, debugH),
                ImVec2(0, 1), ImVec2(1, 0)); // Flip vertical

            ImGui::Separator(); 

            
            ImGui::Text("Refraction (Sous l'eau)");
            ImGui::Image((void*)(intptr_t)texRefract,
                ImVec2(debugW, debugH),
                ImVec2(0, 1), ImVec2(1, 0)); // Flip vertical
        }
        ImGui::End();


       
        waterFBOs->unbindCurrentFrameBuffer(); // returns to the screen buffer
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the scene normally without the crippling (clipPlane = 0)
        renderScene(view, projection, glm::vec4(0, 0, 0, 0));

        // Draw water
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(0, -waterHeight, 0));
        waterObject->draw(model, view, projection);

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
