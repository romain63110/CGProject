#ifndef VIEWER_H
#define VIEWER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <cloud.h>

struct ViewerData {
    GLFWwindow* window;
    GLuint skyboxVAO, skyboxVBO, cubemapTexture, skyboxShader, cloudShader, ufoShader;
    glm::mat4 view, projection;

    //moving camera
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;

    void updateUFO(CloudData& ufo);

    float yaw = -90.0f; // Initialized to look toward -Z
    float pitch = 0.0f;
    float lastX = 400, lastY = 300; // Center of 800x600 window
    bool firstMouse = true;
};


ViewerData initViewer();
GLuint loadCubemap(std::vector<std::string> faces);
std::string loadShaderSource(const std::string& path);
GLuint compileShader(GLenum type, const std::string& source);
GLuint createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void processInput(ViewerData &viewer, float deltaTime);

#endif