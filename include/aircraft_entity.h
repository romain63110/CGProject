#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Node;

class AircraftEntity
{
public:
    AircraftEntity();                          // constructeur
    ~AircraftEntity();                         // destructeur

    bool init(Node* root, const char* rootDir, const char* modelName); // charge JSBSim + ajoute dans scène
    void update(float dt);                     // update simulation + update node

    void setControls(float throttle, float aileron, float elevator, float rudder); // commandes
    Node* getNode() const;                     // accès au node

private:
    Node* node_ = nullptr; // node OpenGL qui représente l’avion

    float throttle_ = 0.0f; // 0..1
    float aileron_ = 0.0f; // -1..1
    float elevator_ = 0.0f; // -1..1
    float rudder_ = 0.0f; // -1..1

    // TODO: ici tu mettras ton wrapper JSBSim (AircraftSim)
};
