#include "afterburner_flame.h"

#include <GL/glew.h>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <vector>

AfterburnerFlame::AfterburnerFlame(Shader* shader, int slices)
    : Shape(shader) 
{
    slices_ = (slices < 6) ? 6 : slices;
    buildConeMesh();
}

AfterburnerFlame::~AfterburnerFlame()
{
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
}

void AfterburnerFlame::buildConeMesh()
{
    std::vector<glm::vec3> verts;

    float radius = 1.0f;
    float z0 = 0.0f;
    float z1 = 1.0f;

    for (int i = 0; i < slices_; i++)
    {
        float a0 = (float)i / (float)slices_ * glm::two_pi<float>();
        float a1 = (float)(i + 1) / (float)slices_ * glm::two_pi<float>();

        glm::vec3 p00(std::cos(a0) * radius, std::sin(a0) * radius, z0);
        glm::vec3 p01(std::cos(a0) * radius, std::sin(a0) * radius, z1);

        glm::vec3 p10(std::cos(a1) * radius, std::sin(a1) * radius, z0);
        glm::vec3 p11(std::cos(a1) * radius, std::sin(a1) * radius, z1);

        verts.push_back(p00);
        verts.push_back(p01);
        verts.push_back(p11);

        verts.push_back(p00);
        verts.push_back(p11);
        verts.push_back(p10);
    }

    vertexCount_ = (int)verts.size();

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
        verts.size() * sizeof(glm::vec3),
        verts.data(),
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
}

void AfterburnerFlame::draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection, glm::vec4& clipPlane)
{
    if (intensity <= 0.001f) return;

    glUseProgram(this->shader_program_);

    
    Shape::draw(model, view, projection, clipPlane);

    
    GLint loc = glGetUniformLocation(this->shader_program_, "uTime");
    glUniform1f(loc, timeSec);

    loc = glGetUniformLocation(this->shader_program_, "uIntensity");
    glUniform1f(loc, intensity);

    loc = glGetUniformLocation(this->shader_program_, "uColor");
    glUniform3f(loc, color.x, color.y, color.z);

    // états OpenGL flamme
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount_);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
