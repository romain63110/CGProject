#include "cloud_shape.h"
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <cmath>

CloudShape::CloudShape(Shader* shader, CloudData* data)
    : Shape(shader), data_(data) {
}

void CloudShape::draw(glm::mat4& model,
    glm::mat4& view,
    glm::mat4& projection,
    glm::vec4&)
{
    glUseProgram(shader_program_);

    glUniformMatrix4fv(
        glGetUniformLocation(shader_program_, "view"),
        1, GL_FALSE, glm::value_ptr(view));

    glUniformMatrix4fv(
        glGetUniformLocation(shader_program_, "projection"),
        1, GL_FALSE, glm::value_ptr(projection));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, data_->textureID);
    glUniform1i(
        glGetUniformLocation(shader_program_, "ufoTexture"), 0);

    glBindVertexArray(data_->VAO);

    float t = (float)glfwGetTime();
    float hover = std::sin(t * 1.5f) * 3.0f;
    float rot = t * 0.8f;
    glUseProgram(shader_program_);

    glUniform3f(
        glGetUniformLocation(shader_program_, "lightPos"),
        0.0f, 100.0f, 0.0f
    );

    glUniform3f(
        glGetUniformLocation(shader_program_, "lightColor"),
        1.0f, 1.0f, 1.0f
    );
    for (auto& inst : data_->instances) {
        glm::mat4 M = model;
        glm::vec3 pos = inst.position;
        pos.y += hover;

        M = glm::translate(M, pos);
        M = glm::rotate(M, rot, glm::vec3(0, 1, 0));
        M = glm::scale(M, glm::vec3(10.0f));

        glUniformMatrix4fv(
            glGetUniformLocation(shader_program_, "model"),
            1, GL_FALSE, glm::value_ptr(M));

        glDrawElements(GL_TRIANGLES,
            (GLsizei)data_->mesh.indices.size(),
            GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
}
