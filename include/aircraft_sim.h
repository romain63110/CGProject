#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace JSBSim { class FGFDMExec; }

class AircraftSim
{
public:
    AircraftSim();
    ~AircraftSim();

    bool init(const std::string& rootDir, const std::string& modelName);
    void setControls(float throttle, float aileron, float elevator, float rudder);
    void step(float dt);

    glm::vec3 getPosition() const;
    glm::quat getOrientation() const;

private:
    JSBSim::FGFDMExec* fdm_ = nullptr;
    float sim_dt_ = 1.0f / 120.0f;
};
