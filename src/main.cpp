#include "viewer.h"
#include "textured_sphere.h"
#include "lighting_sphere.h"
#include "cylinder.h"
#include "texture.h"
#include "node.h"
#include "shader.h"
#include "skybox.h"
#include "triangle.h"

#include <string>

#ifndef SHADER_DIR
#error "SHADER_DIR not defined"
#endif

int main()
{
    Viewer viewer;

    std::string shader_dir = SHADER_DIR;
    std::string texture_dir = "../../../textures/";

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

    // petite sphere de test (optionnelle)
    Shape* sphere2 = new LightingSphere(
        phong_shader,
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    Node* sphere2_node = new Node(glm::mat4(1.0f));
    sphere2_node->add(sphere2);
    viewer.scene_root->add(sphere2_node);

    // ---------------------------
    // Aircraft (fuselage + delta wings)
    // ---------------------------
    Shape* fuselage = new Cylinder(phong_shader, 2.f, 0.5f, 16);

    Node* aircraftNode = new Node(glm::mat4(1.0f));
    aircraftNode->add(fuselage);

    // Wings
    Shape* wingL = new Triangle(phong_shader);
    Shape* wingR = new Triangle(phong_shader);

    // Réglages
    glm::vec3 wingScale(2.2f, 1.2f, 1.0f);
    float wingXOffset = 0.65f;   // distance latérale depuis le fuselage
    float wingZOffset = 0.0f;    // position le long du fuselage
    float wingYOffset = 0.0f;    // hauteur (0 = au milieu)

    // BUT: une arête du triangle // au fuselage
    // - le triangle est défini dans le plan XY, avec sa base alignée sur X.
    // - on le met à plat: Rx(+90°) => plan XZ
    // - on aligne la base (X) sur la longueur (Z) du fuselage: Ry(±90°)

    // Aile droite (pointe vers +X)
    glm::mat4 wingR_mat =
        glm::translate(glm::mat4(1.0f), glm::vec3(+wingXOffset, wingYOffset, wingZOffset)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(+90.0f), glm::vec3(0, 1, 0)) * // base -> Z
        glm::rotate(glm::mat4(1.0f), glm::radians(+90.0f), glm::vec3(1, 0, 0)) * // à plat
        glm::scale(glm::mat4(1.0f), wingScale);

    Node* wingR_node = new Node(wingR_mat);
    wingR_node->add(wingR);

    // Aile gauche (pointe vers -X)
    glm::mat4 wingL_mat =
        glm::translate(glm::mat4(1.0f), glm::vec3(-wingXOffset, wingYOffset, wingZOffset)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0)) * // base -> Z
        glm::rotate(glm::mat4(1.0f), glm::radians(+90.0f), glm::vec3(1, 0, 0)) * // à plat
        glm::scale(glm::mat4(1.0f), wingScale);

    Node* wingL_node = new Node(wingL_mat);
    wingL_node->add(wingL);

    // Attacher les ailes à l'avion
    aircraftNode->add(wingL_node);
    aircraftNode->add(wingR_node);

    // Attacher l'avion à la scène
    viewer.scene_root->add(aircraftNode);
    viewer.aircraft_node = aircraftNode;

    viewer.run();
}
