#include "viewer.h"
#include "texture.h"
#include "node.h"
#include "shader.h"
#include "skybox.h"
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
    std::string texture_dir = "../../../textures/"; //TODO: TEXTURE_DIR

    // Chemins vers vos 6 images
    std::vector<std::string> faces = {
        texture_dir + "Daylight Box_Right.bmp", texture_dir + "Daylight Box_Left.bmp", texture_dir + "Daylight Box_Top.bmp",
        texture_dir + "Daylight Box_Bottom.bmp", texture_dir + "Daylight Box_Front.bmp", texture_dir + "Daylight Box_Back.bmp"
    };

    // 1. Création de la Texture Skybox via le nouveau constructeur
    Texture* skyTexture = new Texture(faces);

    // 2. Création du shader skybox (celui fourni dans la réponse précédente)
    Shader* skyShader = new Shader(shader_dir + "skybox.vert", shader_dir + "skybox.frag");

    // 3. Création de l'objet Skybox
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

    viewer.run();
}