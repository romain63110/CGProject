#include "viewer.h"
#include "texture.h"
#include "node.h"
#include "shader.h"
#include "skybox.h"
#include "cloud.h"
#include "ufo.h"
#include <string>

#ifndef SHADER_DIR
#error "SHADER_DIR not defined"
#endif
#include <terrain.h>

int main()
{
    // create window, add shaders & scene objects, then run rendering loop
    Viewer viewer;

    // get shader directory
    std::string shader_dir = SHADER_DIR;
    std::string texture_dir = "../../textures/"; //TODO: TEXTURE_DIR

    // Chemins vers vos 6 images
    std::vector<std::string> faces = {
        texture_dir + "Daylight Box_Right.bmp", texture_dir + "Daylight Box_Left.bmp", texture_dir + "Daylight Box_Top.bmp",
        texture_dir + "Daylight Box_Bottom.bmp", texture_dir + "Daylight Box_Front.bmp", texture_dir + "Daylight Box_Back.bmp"
    };

    // 1. Cr�ation de la Texture Skybox via le nouveau constructeur
    Texture* skyTexture = new Texture(faces);

    // 2. Cr�ation du shader skybox (celui fourni dans la r�ponse pr�c�dente)
    Shader* skyShader = new Shader(shader_dir + "skybox.vert", shader_dir + "skybox.frag");

    // 3. Cr�ation de l'objet Skybox
    Shape* mySkybox = new Skybox(skyShader, skyTexture);

    // 4. Ajout au Node
    Node* skyNode = new Node(glm::mat4(1.0f));
    skyNode->add(mySkybox);
    viewer.scene_root->add(skyNode);

    Shader* terrainShader = new Shader(shader_dir + "terrain.vert", shader_dir + "terrain.frag");
    Terrain* terrain = new Terrain(terrainShader);
    viewer.scene_root->add(terrain);

    terrain->update(glm::vec3(0));//TODO une liste de fonction / event pou rmetre a jour la pos depuis view
    std::cout << "height: " << terrain->getGlobalHeight(0, 0) << std::endl;
    std::cout << "height: " << terrain->getGlobalHeight(40, 40) << std::endl;

    // ---------------------------
    // Ajout du nuage
    // ---------------------------
    Shader* cloudShader = new Shader(shader_dir + "cloud.vert", shader_dir + "cloud.frag");
    std::string model_dir = "../../models/";
    /*Shape* cloud = new Cloud(model_dir + "nuage.obj", cloudShader);
    
    
    // x = gauche/droite, y = hauteur, z = avant/arrière
    glm::vec3 cloudPosition(10.0f, 50.0f, -50.0f);

    Node* cloudNode = new Node(glm::translate(glm::mat4(1.0f), cloudPosition));
    cloudNode->add(cloud);
    viewer.scene_root->add(cloudNode);*/
   
    // Liste de positions pour tes nuages
    std::vector<glm::vec3> cloudPositions = {
        glm::vec3(20.0f, 50.0f, -50.0f),
        glm::vec3(-15.0f, 60.0f, -25.0f),
        glm::vec3(5.0f, 55.0f, 10.0f)
    };

    std::vector<float> cloudScales = { 1.0f, 2.0f, 0.7f };
    for (auto& pos : cloudPositions) {
        Shape* cloud = new Cloud(model_dir + "nuage.obj", cloudShader);
        Node* cloudNode = new Node(glm::translate(glm::mat4(1.0f), pos));
        cloudNode->add(cloud);
        viewer.scene_root->add(cloudNode);
    } 


    Shader* ufoShader = new Shader(shader_dir + "ufo.vert", shader_dir + "ufo.frag");
    Shape* ufo = new UFO(model_dir + "Soucoupe2.obj", ufoShader);

    glm::mat4 ufoTransform = glm::mat4(1.0f);
    ufoTransform = glm::translate(ufoTransform, glm::vec3(0.0f, 20.0f, -20.0f));
    ufoTransform = glm::scale(ufoTransform, glm::vec3(0.5f)); 

    Node* ufoNode = new Node(ufoTransform);
    ufoNode->add(ufo);
    viewer.scene_root->add(ufoNode);




    viewer.run();
}