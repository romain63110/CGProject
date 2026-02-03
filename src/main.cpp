#include "viewer.h"
#include "texture.h"
#include "node.h"
#include "shader.h"
#include "skybox.h"
#include "runway.h"
#include "obj_model.h"
#include "afterburner_flame.h"
#include "terrain.h"

#include <string>
#include <vector>
#include <cstdlib>

#ifndef SHADER_DIR
#error "SHADER_DIR not defined"
#endif

int main()
{
    Viewer viewer;

    // =========================
    // PATHS
    // =========================
    std::string shader_dir  = SHADER_DIR;
    std::string texture_dir = "../../../textures/";
    std::string model_dir   = "../../../ressources/model/";
    std::string world_dir   = "../../../ressources/world/";

    // =========================
    // SKYBOX
    // =========================
    std::vector<std::string> faces = {
        texture_dir + "Daylight Box_Right.bmp",
        texture_dir + "Daylight Box_Left.bmp",
        texture_dir + "Daylight Box_Top.bmp",
        texture_dir + "Daylight Box_Bottom.bmp",
        texture_dir + "Daylight Box_Front.bmp",
        texture_dir + "Daylight Box_Back.bmp"
    };

    Texture* skyTexture = new Texture(faces);
    Shader* skyShader = new Shader(shader_dir + "skybox.vert",
                                   shader_dir + "skybox.frag");
    Shape* mySkybox = new Skybox(skyShader, skyTexture);

    Node* skyNode = new Node(glm::mat4(1.0f));
    skyNode->add(mySkybox);
    viewer.scene_root->add(skyNode);

    // =========================
    // SHADERS
    // =========================
    Shader* phong_shader = new Shader(shader_dir + "phong.vert",
                                      shader_dir + "phong.frag");

    Shader* color_shader = new Shader(shader_dir + "flat_color.vert",
                                      shader_dir + "flat_color.frag");

    Shader* plane_shader = new Shader(shader_dir + "planeshader.vert",
                                      shader_dir + "planeshader.frag");

    Shader* cloud_shader = new Shader(shader_dir + "cloud.vert",
                                      shader_dir + "cloud.frag");

    Shader* ufo_shader = new Shader(shader_dir + "ufo.vert",
                                    shader_dir + "ufo.frag");

    // =========================
    // RUNWAY
    // =========================
    Runway* runway = new Runway(color_shader, 1000.0f, 12.0f);
    Node* runwayNode = new Node(glm::mat4(1.0f));
    runwayNode->add(runway);
    viewer.scene_root->add(runwayNode);

    // =========================
    // TERRAIN
    // =========================
    Shader* terrainShader = new Shader(shader_dir + "terrain.vert",
                                       shader_dir + "terrain.frag");

    Terrain* terrain = new Terrain(terrainShader);
    viewer.scene_root->add(terrain);
    viewer.terrain_ = terrain;

    // =========================
    // CLOUDS  ✅ CLOUD SHADER
    // =========================
    Node* cloudsRoot = new Node(glm::mat4(1.0f));
    Shape* cloudMesh = new ObjModel(cloud_shader,
                                   world_dir + "cloud.obj");

    for (int i = 0; i < 50; ++i)
    {
        glm::vec3 pos(
            rand() % 800 - 400,
            60.0f + rand() % 30,
            rand() % 800 - 400
        );

        glm::mat4 tr = glm::translate(glm::mat4(1.0f), pos);
        tr = glm::scale(tr, glm::vec3(5.0f));

        Node* cloudNode = new Node(tr);
        cloudNode->add(cloudMesh);

        cloudsRoot->add(cloudNode);
        viewer.cloud_nodes_.push_back(cloudNode);
    }

    viewer.scene_root->add(cloudsRoot);

    // =========================
    // UFO 
    // =========================
    Shape* ufoMesh = new ObjModel(ufo_shader,
                                 world_dir + "UFO.obj");

    glm::mat4 ufoTr = glm::translate(glm::mat4(1.0f),
                                     glm::vec3(0.0f, 120.0f, 0.0f));
    ufoTr = glm::scale(ufoTr, glm::vec3(10.0f));

    Node* ufoNode = new Node(ufoTr);
    ufoNode->add(ufoMesh);

    viewer.scene_root->add(ufoNode);
    viewer.ufo_node_ = ufoNode;

    // =========================
    // AIRCRAFT 
    // =========================
    Node* aircraftNode = new Node(glm::mat4(1.0f));

    Shader* abShader = new Shader(shader_dir + "afterburner.vert",
                                  shader_dir + "afterburner.frag");

    glm::mat4 abL = glm::mat4(1.0f);
    abL = glm::rotate(abL, glm::radians(180.0f), glm::vec3(0, 1, 0));
    abL = glm::rotate(abL, glm::radians(2.0f), glm::vec3(-1, 0, 0));
    abL = glm::scale(abL, glm::vec3(0.08f, 0.08f, 10.f));
    abL = glm::translate(abL, glm::vec3(-1.2f, 8.7f, -1.17f));

    Node* afterburnerNodeL = new Node(abL);
    AfterburnerFlame* flameL = new AfterburnerFlame(abShader, 24);
    flameL->color = glm::vec3(1.0f, 0.1f, 0.05f);
    afterburnerNodeL->add(flameL);

    glm::mat4 abR = glm::mat4(1.0f);
    abR = glm::rotate(abR, glm::radians(180.0f), glm::vec3(0, 1, 0));
    abR = glm::rotate(abR, glm::radians(2.0f), glm::vec3(-1, 0, 0));
    abR = glm::scale(abR, glm::vec3(0.08f, 0.08f, 10.f));
    abR = glm::translate(abR, glm::vec3(1.2f, 8.7f, -1.17f));

    Node* afterburnerNodeR = new Node(abR);
    AfterburnerFlame* flameR = new AfterburnerFlame(abShader, 24);
    flameR->color = glm::vec3(1.0f, 0.1f, 0.05f);
    afterburnerNodeR->add(flameR);

    viewer.afterburnerL_ = flameL;
    viewer.afterburnerR_ = flameR;

    glm::mat4 fix = glm::mat4(1.0f);
    fix = glm::rotate(fix, glm::radians(-90.0f), glm::vec3(0, 1, 0));
    fix = glm::scale(fix, glm::vec3(0.004f));

    Node* planeMeshNode = new Node(fix);
    Shape* planeMesh = new ObjModel(plane_shader,
                                   model_dir + "Plane.obj");
    planeMeshNode->add(planeMesh);

    aircraftNode->add(planeMeshNode);
    aircraftNode->add(afterburnerNodeL);
    aircraftNode->add(afterburnerNodeR);

    viewer.scene_root->add(aircraftNode);
    viewer.aircraft_.node = aircraftNode;

    // =========================
    // RUN
    // =========================
    viewer.run();
}
