#include "../include/viewer.h"
#include "../include/stb_image.h"
#include <iostream>
#include <fstream>
#include <cmath>


//creats window
ViewerData initViewer() {
    ViewerData data;
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }
    data.window = glfwCreateWindow(800, 600, "Cloudy Skybox", NULL, NULL);
    if (!data.window) {
        glfwTerminate();
        std::cerr << "Failed to create GLFW window" << std::endl;
        exit(-1);
    }
    glfwMakeContextCurrent(data.window);
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        exit(-1);
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND); //dont overwrite the pixel color => blend
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Skybox vertices
    float skyboxVertices[] = {
        -500.0f,  500.0f, -500.0f, -500.0f, -500.0f, -500.0f,  500.0f, -500.0f, -500.0f,
         500.0f, -500.0f, -500.0f,  500.0f,  500.0f, -500.0f, -500.0f,  500.0f, -500.0f,
        -500.0f, -500.0f,  500.0f, -500.0f, -500.0f, -500.0f, -500.0f,  500.0f, -500.0f,
        -500.0f,  500.0f, -500.0f, -500.0f,  500.0f,  500.0f, -500.0f, -500.0f,  500.0f,
         500.0f, -500.0f, -500.0f,  500.0f, -500.0f,  500.0f,  500.0f,  500.0f,  500.0f,
         500.0f,  500.0f,  500.0f,  500.0f,  500.0f, -500.0f,  500.0f, -500.0f, -500.0f,
        -500.0f, -500.0f,  500.0f, -500.0f,  500.0f,  500.0f,  500.0f,  500.0f,  500.0f,
         500.0f,  500.0f,  500.0f,  500.0f, -500.0f,  500.0f, -500.0f, -500.0f,  500.0f,
        -500.0f,  500.0f, -500.0f,  500.0f,  500.0f, -500.0f,  500.0f,  500.0f,  500.0f,
         500.0f,  500.0f,  500.0f, -500.0f,  500.0f,  500.0f, -500.0f,  500.0f, -500.0f,
        -500.0f, -500.0f, -500.0f, -500.0f, -500.0f,  500.0f,  500.0f, -500.0f, -500.0f,
         500.0f, -500.0f, -500.0f, -500.0f, -500.0f,  500.0f,  500.0f, -500.0f,  500.0f
    };

    glGenVertexArrays(1, &data.skyboxVAO);
    glBindVertexArray(data.skyboxVAO);
    glGenBuffers(1, &data.skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, data.skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    std::vector<std::string> faces = {"texture/right.png", "texture/left.png", "texture/top.png", "texture/bottom.png", "texture/front.png", "texture/back.png"};
    data.cubemapTexture = loadCubemap(faces);
    std::cout << "Cubemap loaded" << std::endl;

    data.skyboxShader = createShaderProgram("shaders/skybox.vert", "shaders/skybox.frag");
    data.cloudShader = createShaderProgram("shaders/cloud.vert", "shaders/cloud.frag");
    data.ufoShader = createShaderProgram("shaders/ufo.vert", "shaders/ufo.frag");
    std::cout << "Shaders created" << std::endl;

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    //data.view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    data.projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);

    data.cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
    data.cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    data.cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

    return data;
}

GLuint loadCubemap(std::vector<std::string> faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (unsigned int i = 0; i < faces.size(); i++) {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return textureID;
}

std::string loadShaderSource(const std::string& path) {
    std::ifstream file(path);
    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return source;
}

GLuint compileShader(GLenum type, const std::string& source) {
    const char* src = source.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Shader compilation failed: " << infoLog << std::endl;
    }
    return shader;
}

GLuint createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexSource = loadShaderSource(vertexPath);
    std::string fragmentSource = loadShaderSource(fragmentPath);
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "Program linking failed: " << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    ViewerData* viewer = (ViewerData*)glfwGetWindowUserPointer(window);

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (viewer->firstMouse) {
        viewer->lastX = xpos;
        viewer->lastY = ypos;
        viewer->firstMouse = false;
    }

    float xoffset = xpos - viewer->lastX;
    float yoffset = viewer->lastY - ypos; 
    viewer->lastX = xpos;
    viewer->lastY = ypos;

    float sensitivity = 0.1f; 
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    viewer->yaw   += xoffset;
    viewer->pitch += yoffset;

    if (viewer->pitch > 89.0f) viewer->pitch = 89.0f;
    if (viewer->pitch < -89.0f) viewer->pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(viewer->yaw)) * cos(glm::radians(viewer->pitch));
    front.y = sin(glm::radians(viewer->pitch));
    front.z = sin(glm::radians(viewer->yaw)) * cos(glm::radians(viewer->pitch));
    viewer->cameraFront = glm::normalize(front);
}


void processInput(ViewerData &viewer, float deltaTime) {
    float cameraSpeed = 50.0f * deltaTime; 

    if (glfwGetKey(viewer.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(viewer.window, true);
    
    // Movement logic... (W, A, S, D, Q, E)
    if (glfwGetKey(viewer.window, GLFW_KEY_W) == GLFW_PRESS)
        viewer.cameraPos += cameraSpeed * viewer.cameraFront;
    if (glfwGetKey(viewer.window, GLFW_KEY_S) == GLFW_PRESS)
        viewer.cameraPos -= cameraSpeed * viewer.cameraFront;
    if (glfwGetKey(viewer.window, GLFW_KEY_A) == GLFW_PRESS)
        viewer.cameraPos -= glm::normalize(glm::cross(viewer.cameraFront, viewer.cameraUp)) * cameraSpeed;
    if (glfwGetKey(viewer.window, GLFW_KEY_D) == GLFW_PRESS)
        viewer.cameraPos += glm::normalize(glm::cross(viewer.cameraFront, viewer.cameraUp)) * cameraSpeed;
    if (glfwGetKey(viewer.window, GLFW_KEY_E) == GLFW_PRESS)
        viewer.cameraPos += cameraSpeed * viewer.cameraUp;
    if (glfwGetKey(viewer.window, GLFW_KEY_Q) == GLFW_PRESS)
        viewer.cameraPos -= cameraSpeed * viewer.cameraUp;
}

void ViewerData::updateUFO(CloudData& ufo) {
    float time = (float)glfwGetTime();
    //sin(time * speed) * height
    float hoverY = sin(time * 1.5f) * 3.0f; 
    float rotation = time * 0.8f;

    for (auto& inst : ufo.instances) {
        glm::mat4 model = glm::mat4(1.0f);
        //posiiton & hover
        glm::vec3 animatedPos = inst.position;
        animatedPos.y += hoverY;
        model = glm::translate(model, animatedPos);
        model = glm::rotate(model, rotation, glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(10.0f));
        glUniformMatrix4fv(glGetUniformLocation(this->ufoShader, "model"), 1, GL_FALSE, &model[0][0]);
    }
}