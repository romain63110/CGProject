#include "../include/viewer.h"
#include "../include/cloud.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <ostream>


/* How it works :
main.cpp asks viewer.cpp to open a window ->asks cloud.cpp to load a model and pick 20 random spots.
The Loop Starts:
-viewer.cpp updates where you are looking based on the mouse.
-The CPU sends 20 "Model Matrices" to the GPU.
-The GPU runs the Shaders to draw the pixels.
-The window swaps buffers to show you the new frame
*/


int main() {
    ViewerData viewer = initViewer();
    CloudData clouds = initCloudField("cloud.obj", 50); //load clouds
    CloudData ufo = initCloudField("UFO.obj", 1); //loads an ufo =>same functions as clouds but will have different shaders
    //std::cout << "UFO instances count: " << ufo.instances.size() << std::endl;
    //std::cout << "UFO indices count: " << ufo.mesh.indices.size() << std::endl;

    glfwSetWindowUserPointer(viewer.window, &viewer);
    glfwSetInputMode(viewer.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(viewer.window, mouse_callback);

    float deltaTime = 0.0f; 
    float lastFrame = 0.0f;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(viewer.window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(viewer, deltaTime);
        viewer.view = glm::lookAt(viewer.cameraPos, viewer.cameraPos + viewer.cameraFront, viewer.cameraUp);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render Skybox
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glUseProgram(viewer.skyboxShader);
        glm::mat4 skyView = glm::mat4(glm::mat3(viewer.view));
        glUniformMatrix4fv(glGetUniformLocation(viewer.skyboxShader, "view"), 1, GL_FALSE, glm::value_ptr(skyView));
        glUniformMatrix4fv(glGetUniformLocation(viewer.skyboxShader, "projection"), 1, GL_FALSE, glm::value_ptr(viewer.projection));
        glBindVertexArray(viewer.skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, viewer.cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);


         // Render UFO
        glUseProgram(viewer.ufoShader); 
        glUniform3f(glGetUniformLocation(viewer.ufoShader, "lightColor"), 1.0f, 1.0f, 1.0f); // pure White
        glUniform3f(glGetUniformLocation(viewer.ufoShader, "lightPos"), 0.0f, 100.0f, 0.0f);   // high above
        viewer.updateUFO(ufo);
        glDepthMask(GL_TRUE);

        //projection and view change with the camera
        glUniformMatrix4fv(glGetUniformLocation(viewer.ufoShader, "view"), 1, GL_FALSE, glm::value_ptr(viewer.view));
        glUniformMatrix4fv(glGetUniformLocation(viewer.ufoShader, "projection"), 1, GL_FALSE, glm::value_ptr(viewer.projection));

        glUseProgram(viewer.ufoShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ufo.textureID);
        glUniform1i(glGetUniformLocation(viewer.ufoShader, "ufoTexture"), 0);

        glBindVertexArray(ufo.VAO);
        glDrawElements(GL_TRIANGLES, ufo.mesh.indices.size(), GL_UNSIGNED_INT, 0);


        // Render Cloud Instances 
        glUseProgram(viewer.cloudShader);
        glDepthMask(GL_FALSE); 

        glUniformMatrix4fv(glGetUniformLocation(viewer.cloudShader, "view"), 1, GL_FALSE, glm::value_ptr(viewer.view));
        glUniformMatrix4fv(glGetUniformLocation(viewer.cloudShader, "projection"), 1, GL_FALSE, glm::value_ptr(viewer.projection));
        glBindVertexArray(clouds.VAO);
        for (const auto& instance : clouds.instances) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, instance.position);
            model = glm::rotate(model, glm::radians(instance.rotation), glm::vec3(0, 1, 0));
            model = glm::scale(model, glm::vec3(5.0f)); 
    
            glUniformMatrix4fv(glGetUniformLocation(viewer.cloudShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawElements(GL_TRIANGLES, clouds.mesh.indices.size(), GL_UNSIGNED_INT, 0);
        }
        glDepthMask(GL_TRUE);

        glfwSwapBuffers(viewer.window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}