#include "aircraft_entity.h"
#include "node.h"
#include "shape.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <FGFDMExec.h>                 
#include <input_output/FGPropertyManager.h>

AircraftEntity::AircraftEntity() {}

AircraftEntity::~AircraftEntity()
{
    // si tu alloues node_ avec new, tu dois delete ici
}

bool AircraftEntity::init(Node* root, const char* rootDir, const char* modelName)
{
    node_ = new Node(glm::mat4(1.0f)); // node avion (identité au début)

    // Pour l’instant : placeholder simple (cylindre, sphere, etc.)
    // Shape* placeholder = new Cylinder(...); 
    // node_->add(placeholder);

    root->add(node_); // ajoute l’avion dans la scène

    // TODO: ici tu initialises JSBSim (LoadModel etc.)
    return true;
}

void AircraftEntity::setControls(float throttle, float aileron, float elevator, float rudder)
{
    throttle_ = throttle;
    aileron_ = aileron;
    elevator_ = elevator;
    rudder_ = rudder;
}

void AircraftEntity::update(float dt)
{
    // TODO: ici tu fais sim.step(dt) et tu récupères position+rotation depuis JSBSim

    glm::vec3 pos(0, 0, 0);  // <- à remplacer par JSBSim position
    glm::quat rot(1, 0, 0, 0); // <- à remplacer par JSBSim orientation

    glm::mat4 model =
        glm::translate(glm::mat4(1.0f), pos) *
        glm::mat4_cast(rot);

    node_->set_transform(model); // ton node suit l’avion
}

Node* AircraftEntity::getNode() const
{
    return node_;
}
