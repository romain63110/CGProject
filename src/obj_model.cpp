#include "obj_model.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>

struct VertexPN
{
    glm::vec3 pos;
    glm::vec3 normal;
};

// Petit struct interne pour stocker les indices d’un vertex de face
struct FaceIndex
{
    int v = 0;   // index position
    int vt = 0;  // index texcoord (optionnel)
    int vn = 0;  // index normal (optionnel)
};

// Parse un token OBJ (ex: "3", "3//2", "3/4", "3/4/2")
static FaceIndex parseFaceToken(const std::string& token)
{
    FaceIndex idx;

    // Cas simple : "v"
    size_t p1 = token.find('/');
    if (p1 == std::string::npos)
    {
        idx.v = std::stoi(token);
        return idx;
    }

    // Sinon on a au moins "v/..."
    std::string sV = token.substr(0, p1);
    idx.v = !sV.empty() ? std::stoi(sV) : 0;

    size_t p2 = token.find('/', p1 + 1);

    // Cas "v/vt" (un seul slash)
    if (p2 == std::string::npos)
    {
        std::string sVT = token.substr(p1 + 1);
        idx.vt = !sVT.empty() ? std::stoi(sVT) : 0;
        return idx;
    }

    // Cas "v//vn" ou "v/vt/vn"
    std::string sVT = token.substr(p1 + 1, p2 - p1 - 1);
    std::string sVN = token.substr(p2 + 1);

    idx.vt = !sVT.empty() ? std::stoi(sVT) : 0;
    idx.vn = !sVN.empty() ? std::stoi(sVN) : 0;

    return idx;
}

// Convertit index OBJ vers index C++ (OBJ commence à 1, et accepte les négatifs)
static int fixObjIndex(int idx, int size)
{
    if (idx > 0)
        return idx - 1; // OBJ 1-based
    if (idx < 0)
        return size + idx; // ex: -1 => dernier élément
    return -1;
}

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
        // ignore lignes vides et commentaires
        if (line.empty() || line[0] == '#')
            continue;

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
            // Lire TOUS les tokens de la face
            std::vector<FaceIndex> face;
            std::string token;

            while (iss >> token)
            {
                FaceIndex fi = parseFaceToken(token);
                face.push_back(fi);
            }

            // Une face doit avoir au moins 3 sommets
            if ((int)face.size() < 3)
                continue;

            // Triangulation en fan :
            // (0,1,2) (0,2,3) (0,3,4) ...
            for (int i = 1; i < (int)face.size() - 1; i++)
            {
                FaceIndex f0 = face[0];
                FaceIndex f1 = face[i];
                FaceIndex f2 = face[i + 1];

                FaceIndex tri[3] = { f0, f1, f2 };

                for (int k = 0; k < 3; k++)
                {
                    int vIndex = fixObjIndex(tri[k].v, (int)positions.size());
                    int vnIndex = fixObjIndex(tri[k].vn, (int)normals.size());

                    if (vIndex < 0 || vIndex >= (int)positions.size())
                    {
                        // mauvais fichier ou parsing -> on skip
                        continue;
                    }

                    glm::vec3 pos = positions[vIndex];

                    glm::vec3 nrm = glm::vec3(0, 1, 0);
                    if (vnIndex >= 0 && vnIndex < (int)normals.size())
                        nrm = normals[vnIndex];

                    vertices.push_back({ pos, nrm });
                }
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
    glBindVertexArray(0);
}
