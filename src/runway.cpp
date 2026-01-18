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
    glBindVertexArray(VAO);

    Shape::draw(model, view, projection);

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}
