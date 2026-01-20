#include "viewer.h"
#include "textured_sphere.h"
#include "lighting_sphere.h"
#include "cylinder.h"
#include "texture.h"
#include "node.h"
#include "shader.h"
#include "skybox.h"
#include "triangle.h"
#include "runway.h"
#include "obj_model.h"   
#include "afterburner_flame.h"


#include <string>

#ifndef SHADER_DIR
#error "SHADER_DIR not defined"
#endif

int main()
{
    Viewer viewer;

    std::string shader_dir = SHADER_DIR;
    std::string texture_dir = "../../../textures/";
    std::string model_dir = "../../../ressources/model/"; 

    std::vector<std::string> faces = {
        texture_dir + "Daylight Box_Right.bmp",
        texture_dir + "Daylight Box_Left.bmp",
        texture_dir + "Daylight Box_Top.bmp",
        texture_dir + "Daylight Box_Bottom.bmp",
        texture_dir + "Daylight Box_Front.bmp",
        texture_dir + "Daylight Box_Back.bmp"
    };

    Texture* skyTexture = new Texture(faces);
    Shader* skyShader = new Shader(shader_dir + "skybox.vert", shader_dir + "skybox.frag");
    Shape* mySkybox = new Skybox(skyShader, skyTexture);

    Node* skyNode = new Node(glm::mat4(1.0f));
    skyNode->add(mySkybox);
    viewer.scene_root->add(skyNode);

    Shader* phong_shader = new Shader(shader_dir + "phong.vert", shader_dir + "phong.frag");
    Shader* color_shader = new Shader(shader_dir + "flat_color.vert", shader_dir + "flat_color.frag");

    Runway* runway = new Runway(color_shader, 160000.0f, 12.0f);
    Node* runwayNode = new Node(glm::mat4(1.0f));
    runwayNode->add(runway);
    viewer.scene_root->add(runwayNode);

    Shape* sphere2 = new LightingSphere(
        phong_shader,
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    Node* sphere2_node = new Node(glm::mat4(1.0f));
    sphere2_node->add(sphere2);
    viewer.scene_root->add(sphere2_node);

    Node* aircraftNode = new Node(glm::mat4(1.0f));

    
    Shader* abShader = new Shader(shader_dir + "afterburner.vert",
        shader_dir + "afterburner.frag");

    // ---------- moteur gauche ----------
    glm::mat4 abL = glm::mat4(1.0f);

    abL = glm::rotate(abL, glm::radians(180.0f), glm::vec3(0, 1, 0));
    abL = glm::rotate(abL, glm::radians(2.0f), glm::vec3(-1, 0, 0));
    abL = glm::scale(abL, glm::vec3(0.08f, 0.08f, 10.f));
    abL = glm::translate(abL, glm::vec3(-0.09f, 0.2f, -1.1f));   // X = gauche

    Node* afterburnerNodeL = new Node(abL);
    AfterburnerFlame* flameL = new AfterburnerFlame(abShader, 24);
    flameL->color = glm::vec3(1.0f, 0.1f, 0.05f); // rouge
    afterburnerNodeL->add(flameL);


    // ---------- moteur droit ----------
    glm::mat4 abR = glm::mat4(1.0f);
   
    abR = glm::rotate(abR, glm::radians(180.0f), glm::vec3(0, 1, 0));
    abR = glm::rotate(abR, glm::radians(2.0f), glm::vec3(-1, 0, 0));
    abR = glm::scale(abR, glm::vec3(0.08f, 0.08f, 10.f));
    abR = glm::translate(abR, glm::vec3(+0.1f, 0.26f, 1.f));

    Node* afterburnerNodeR = new Node(abR);

    AfterburnerFlame* flameR = new AfterburnerFlame(abShader, 24);
    flameR->color = glm::vec3(1.0f, 0.1f, 0.05f); 
    afterburnerNodeR->add(flameR);

    // dans viewer : stocker les 2 pointeurs
    viewer.afterburnerL_ = flameL;
    viewer.afterburnerR_ = flameR;



    // fix = glm::rotate(fix, glm::radians(-90.0f), glm::vec3(1,0,0)); // Z-up -> Y-up

    glm::mat4 fix = glm::mat4(1.0f);
    fix = glm::rotate(fix, glm::radians(-90.0f), glm::vec3(0, 1, 0));
    fix = glm::scale(fix, glm::vec3(0.004f));

    Node* planeMeshNode = new Node(fix);

    Shape* planeMesh = new ObjModel(phong_shader, model_dir + "Plane.obj");
    planeMeshNode->add(planeMesh);

    aircraftNode->add(planeMeshNode);
    aircraftNode->add(afterburnerNodeL);
    aircraftNode->add(afterburnerNodeR);

    // Attacher l'avion à la scène
    viewer.scene_root->add(aircraftNode);
    viewer.aircraft_.node = aircraftNode;

    viewer.run();
}
