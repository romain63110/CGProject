#include "aircraft_sim.h"


#include <simgear/misc/sg_path.hxx>

#include <iostream>
#include <memory> // std::shared_ptr

#include <FGFDMExec.h>          // moteur JSBSim
#include <input_output/FGPropertyManager.h>  // propriétés JSBSim (dans ton dossier)

#include <glm/gtc/quaternion.hpp> // glm::quat / angleAxis

AircraftSim::AircraftSim() {}

AircraftSim::~AircraftSim()
{
    delete fdm_;      // libère la mémoire JSBSim
    fdm_ = nullptr;   // évite un pointeur dangling
}

bool AircraftSim::init(const std::string& rootDir, const std::string& modelName)
{
    fdm_ = new JSBSim::FGFDMExec(); // crée l’objet principal JSBSim

    fdm_->SetRootDir(SGPath(rootDir));
    fdm_->Setdt(sim_dt_);           // fixe un dt stable (ex: 1/120)

    bool ok = fdm_->LoadModel(modelName); // charge le modèle avion (ex: "f104")
    if (!ok)
    {
        std::cerr << "JSBSim LoadModel failed: " << modelName << std::endl;
        return false;
    }

    fdm_->RunIC(); // initialise les conditions initiales
    return true;
}

void AircraftSim::setControls(float throttle, float aileron, float elevator, float rudder)
{
    auto pm = fdm_->GetPropertyManager(); // shared_ptr<FGPropertyManager>

    // throttle moteur (0..1)
    pm->GetNode("fcs/throttle-cmd-norm")->setDoubleValue(throttle);

    // commandes de vol (-1..1)
    pm->GetNode("fcs/aileron-cmd-norm")->setDoubleValue(aileron);
    pm->GetNode("fcs/elevator-cmd-norm")->setDoubleValue(elevator);
    pm->GetNode("fcs/rudder-cmd-norm")->setDoubleValue(rudder);
}

void AircraftSim::step(float dt)
{
    static float accumulator = 0.0f; // stocke le temps réel accumulé
    accumulator += dt;

    while (accumulator >= sim_dt_)   // tant qu’on peut simuler un step
    {
        fdm_->Run();                 // exécute un pas de simulation JSBSim
        accumulator -= sim_dt_;      // on retire le temps simulé
    }
}

glm::vec3 AircraftSim::getPosition() const
{
    auto pm = fdm_->GetPropertyManager(); // shared_ptr<FGPropertyManager>

    // ?? Ces propriétés peuvent varier selon le modèle.
    float x = (float)pm->GetNode("position/local-x")->getDoubleValue();
    float y = (float)pm->GetNode("position/local-y")->getDoubleValue();
    float z = (float)pm->GetNode("position/local-z")->getDoubleValue();

    return glm::vec3(x, y, z);
}

glm::quat AircraftSim::getOrientation() const
{
    auto pm = fdm_->GetPropertyManager(); // shared_ptr<FGPropertyManager>

    float roll = (float)pm->GetNode("attitude/phi-rad")->getDoubleValue();
    float pitch = (float)pm->GetNode("attitude/theta-rad")->getDoubleValue();
    float yaw = (float)pm->GetNode("attitude/psi-rad")->getDoubleValue();

    // conversion Euler -> Quaternion
    glm::quat qYaw = glm::angleAxis(yaw, glm::vec3(0, 1, 0));
    glm::quat qPitch = glm::angleAxis(pitch, glm::vec3(1, 0, 0));
    glm::quat qRoll = glm::angleAxis(roll, glm::vec3(0, 0, 1));

    return qYaw * qPitch * qRoll;
}
