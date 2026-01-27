#include "obj_model.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cctype>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

struct VertexPNT
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct FaceIndex
{
    int v = 0;
    int vt = 0;
    int vn = 0;
};

struct Material
{
    glm::vec3 Ka = glm::vec3(1.0f);
    glm::vec3 Kd = glm::vec3(1.0f);
    glm::vec3 Ks = glm::vec3(0.0f);
    float Ns = 32.0f;
};

static std::string getDirectoryFromPath(const std::string& path)
{
    size_t slash = path.find_last_of("/\\");
    if (slash == std::string::npos) return "";
    return path.substr(0, slash + 1);
}

static FaceIndex parseFaceToken(const std::string& token)
{
    FaceIndex idx;

    size_t p1 = token.find('/');
    if (p1 == std::string::npos)
    {
        idx.v = std::stoi(token);
        return idx;
    }

    std::string sV = token.substr(0, p1);
    idx.v = !sV.empty() ? std::stoi(sV) : 0;

    size_t p2 = token.find('/', p1 + 1);

    if (p2 == std::string::npos)
    {
        std::string sVT = token.substr(p1 + 1);
        idx.vt = !sVT.empty() ? std::stoi(sVT) : 0;
        return idx;
    }

    std::string sVT = token.substr(p1 + 1, p2 - p1 - 1);
    std::string sVN = token.substr(p2 + 1);

    idx.vt = !sVT.empty() ? std::stoi(sVT) : 0;
    idx.vn = !sVN.empty() ? std::stoi(sVN) : 0;

    return idx;
}

static int fixObjIndex(int idx, int size)
{
    if (idx > 0) return idx - 1;
    if (idx < 0) return size + idx;
    return -1;
}

static std::unordered_map<std::string, Material> loadMTL(const std::string& mtlPath)
{
    std::unordered_map<std::string, Material> mats;

    std::ifstream file(mtlPath);
    if (!file.is_open())
    {
        std::cout << "[MTL] FAIL open " << mtlPath << "\n";
        return mats;
    }

    std::cout << "[MTL] OPEN " << mtlPath << "\n";

    std::string line;
    std::string currentName;
    Material currentMat;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "newmtl")
        {
            if (!currentName.empty())
                mats[currentName] = currentMat;

            iss >> currentName;
            currentMat = Material();
            std::cout << "[MTL] material " << currentName << "\n";
        }
        else if (type == "Ka")
        {
            iss >> currentMat.Ka.r >> currentMat.Ka.g >> currentMat.Ka.b;
        }
        else if (type == "Kd")
        {
            iss >> currentMat.Kd.r >> currentMat.Kd.g >> currentMat.Kd.b;
        }
        else if (type == "Ks")
        {
            iss >> currentMat.Ks.r >> currentMat.Ks.g >> currentMat.Ks.b;
        }
        else if (type == "Ns")
        {
            iss >> currentMat.Ns;
        }
        else if (type == "map_Kd")
        {
            std::string tex;
            iss >> tex;
            std::cout << "[MTL] map_Kd FOUND but NOT LOADED: " << tex << "\n";
        }
    }

    if (!currentName.empty())
        mats[currentName] = currentMat;

    std::cout << "[MTL] materials count = " << mats.size() << "\n";
    return mats;
}

ObjModel::ObjModel(Shader* shader_program, const std::string& objPath)
    : Shape(shader_program)
{
    std::cout << "[OBJ] load " << objPath << "\n";

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;

    std::string folder = getDirectoryFromPath(objPath);
    std::unordered_map<std::string, Material> materials;
    std::unordered_map<std::string, std::vector<VertexPNT>> groups;

    std::string currentMaterialName = "default";

    std::ifstream file(objPath);
    if (!file.is_open())
    {
        std::cout << "[OBJ] FAIL open\n";
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "mtllib")
        {
            std::string mtlFile;
            iss >> mtlFile;
            materials = loadMTL(folder + mtlFile);
        }
        else if (type == "usemtl")
        {
            iss >> currentMaterialName;
            std::cout << "[OBJ] usemtl " << currentMaterialName << "\n";
        }
        else if (type == "v")
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
        else if (type == "vt")
        {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            texcoords.push_back(uv);
        }
        else if (type == "f")
        {
            std::vector<FaceIndex> face;
            std::string token;
            while (iss >> token)
                face.push_back(parseFaceToken(token));

            for (int i = 1; i < (int)face.size() - 1; i++)
            {
                FaceIndex tri[3] = { face[0], face[i], face[i + 1] };

                for (int k = 0; k < 3; k++)
                {
                    int vIndex = fixObjIndex(tri[k].v, positions.size());
                    int vnIndex = fixObjIndex(tri[k].vn, normals.size());
                    int vtIndex = fixObjIndex(tri[k].vt, texcoords.size());

                    glm::vec3 pos = positions[vIndex];
                    glm::vec3 nrm = (vnIndex >= 0) ? normals[vnIndex] : glm::vec3(0, 1, 0);
                    glm::vec2 uv = (vtIndex >= 0) ? texcoords[vtIndex] : glm::vec2(0);

                    groups[currentMaterialName].push_back({ pos, nrm, uv });
                }
            }
        }
    }

    for (auto& it : groups)
    {
        SubMesh sm;
        sm.materialName = it.first;
        sm.vertexCount = (int)it.second.size();

        Material m = materials.count(it.first) ? materials[it.first] : Material();
        sm.Kd = m.Kd;
        sm.Ks = m.Ks;
        sm.Ns = m.Ns;

        glGenVertexArrays(1, &sm.VAO);
        glBindVertexArray(sm.VAO);

        glGenBuffers(1, &sm.VBO);
        glBindBuffer(GL_ARRAY_BUFFER, sm.VBO);
        glBufferData(GL_ARRAY_BUFFER, it.second.size() * sizeof(VertexPNT), it.second.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNT), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNT), (void*)offsetof(VertexPNT, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPNT), (void*)offsetof(VertexPNT, uv));

        glBindVertexArray(0);
        submeshes_.push_back(sm);
    }

    std::cout << "[OBJ] submeshes = " << submeshes_.size() << "\n";
}

void ObjModel::draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection)
{
    glUseProgram(shader_program_);

    glUniformMatrix4fv(glGetUniformLocation(shader_program_, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader_program_, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader_program_, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    GLint locKd = glGetUniformLocation(shader_program_, "uKd");
    GLint locKs = glGetUniformLocation(shader_program_, "uKs");
    GLint locNs = glGetUniformLocation(shader_program_, "uNs");

    for (auto& sm : submeshes_)
    {
        glUniform3fv(locKd, 1, glm::value_ptr(sm.Kd));
        glUniform3fv(locKs, 1, glm::value_ptr(sm.Ks));
        glUniform1f(locNs, sm.Ns);

        glBindVertexArray(sm.VAO);
        glDrawArrays(GL_TRIANGLES, 0, sm.vertexCount);
        glBindVertexArray(0);
    }
}
