#include "obj_model.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>

struct VertexPN
{
    glm::vec3 pos;
    glm::vec3 normal;
};

ObjModel::ObjModel(Shader* shader_program, const std::string& objPath)
    : Shape(shader_program)
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<VertexPN> vertices;

    std::ifstream file(objPath);
    if (!file.is_open())
    {
        std::cerr << "[ObjModel] Cannot open: " << objPath << "\n";
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v")
        {
            glm::vec3 p;
            iss >> p.x >> p.y >> p.z;
            positions.push_back(p);
        }
        else if (type == "vn")
        {
            glm::vec3 n;
            iss >> n.x >> n.y >> n.z;
            normals.push_back(glm::normalize(n));
        }
        else if (type == "f")
        {
            // on lit 3 sommets (triangles)
            for (int i = 0; i < 3; i++)
            {
                std::string token;
                iss >> token;

                int vIndex = 0, vtIndex = 0, vnIndex = 0;

                // token = "v//vn" ou "v/vt/vn" ou "v/vt"
                size_t p1 = token.find('/');
                size_t p2 = token.find('/', p1 + 1);

                if (p1 == std::string::npos)
                {
                    vIndex = std::stoi(token);
                }
                else
                {
                    vIndex = std::stoi(token.substr(0, p1));

                    if (p2 != std::string::npos)
                    {
                        std::string between = token.substr(p1 + 1, p2 - p1 - 1);
                        std::string after = token.substr(p2 + 1);

                        if (!between.empty())
                            vtIndex = std::stoi(between);

                        if (!after.empty())
                            vnIndex = std::stoi(after);
                    }
                }

                glm::vec3 pos = positions[vIndex - 1];
                glm::vec3 nrm = (vnIndex > 0 && vnIndex <= (int)normals.size())
                    ? normals[vnIndex - 1]
                    : glm::vec3(0, 1, 0);

                vertices.push_back({ pos, nrm });
            }
        }
    }

    vertexCount = (int)vertices.size();
    if (vertexCount == 0)
    {
        std::cerr << "[ObjModel] No vertices loaded from: " << objPath << "\n";
        return;
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexPN), vertices.data(), GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPN), (void*)0);

    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPN), (void*)offsetof(VertexPN, normal));

    glBindVertexArray(0);
}

ObjModel::~ObjModel()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void ObjModel::draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection)
{
    glUseProgram(this->shader_program_);

    GLint locModel = glGetUniformLocation(this->shader_program_, "model");
    GLint locView = glGetUniformLocation(this->shader_program_, "view");
    GLint locProj = glGetUniformLocation(this->shader_program_, "projection");

    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(locView, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(locProj, 1, GL_FALSE, &projection[0][0]);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}
