#include "runway.h"

Runway::Runway(Shader* shader_program, float length, float width)
    : Shape(shader_program)
{
    float L = length * 0.5f;
    float W = width * 0.5f;

    GLfloat vertices[] = {
        -W, 0.0f, -L,
         W, 0.0f, -L,
         W, 0.0f,  L,

        -W, 0.0f, -L,
         W, 0.0f,  L,
        -W, 0.0f,  L
    };

    vertexCount = 6;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

Runway::~Runway()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Runway::draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection)
{
    glUseProgram(this->shader_program_);

    GLint locModel = glGetUniformLocation(this->shader_program_, "model");
    GLint locView = glGetUniformLocation(this->shader_program_, "view");
    GLint locProj = glGetUniformLocation(this->shader_program_, "projection");

    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(locView, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(locProj, 1, GL_FALSE, &projection[0][0]);

    GLint locColor = glGetUniformLocation(this->shader_program_, "color");
    if (locColor >= 0)
        glUniform3f(locColor, 0.15f, 0.15f, 0.15f);

    GLint locUColor = glGetUniformLocation(this->shader_program_, "uColor");
    if (locUColor >= 0)
        glUniform3f(locUColor, 0.15f, 0.15f, 0.15f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

