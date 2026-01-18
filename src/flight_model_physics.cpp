#include "flight_model_physics.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

void FlightModelPhysics::step(Aircraft& ac, const AircraftControls& u, float dt)
{
    glm::vec3 forward = ac.orientation * glm::vec3(0, 0, -1);

    float roll_rate = glm::radians(roll_rate_deg);
    glm::quat qRoll = glm::angleAxis(roll_rate * u.aileron * dt, forward);

    ac.orientation = glm::normalize(qRoll * ac.orientation);

    forward = ac.orientation * glm::vec3(0, 0, -1);

    glm::vec3 Ft = forward * (u.throttle * Tmax);

    glm::vec3 v = ac.velocity;
    float speed = glm::length(v);

    glm::vec3 Fd(0.0f);
    if (speed > 0.001f)
        Fd = -drag_k * v * speed;

    glm::vec3 Fg = glm::vec3(0.0f, -mass * 9.81f, 0.0f);

    glm::vec3 a = (Ft + Fd + Fg) / mass;

    ac.velocity += a * dt;
    ac.position += ac.velocity * dt;

    float runwayLength = 80.0f;
    float runwayWidth = 12.0f;
    float halfL = runwayLength * 0.5f;
    float halfW = runwayWidth * 0.5f;

    bool onRunway =
        (ac.position.x >= -halfW && ac.position.x <= halfW) &&
        (ac.position.z >= -halfL && ac.position.z <= halfL);

    if (onRunway && ac.position.y < 0.5f)
    {
        ac.position.y = 0.5f;
        if (ac.velocity.y < 0.0f) ac.velocity.y = 0.0f;

        ac.velocity.x *= 0.99f;
        ac.velocity.z *= 0.99f;
    }

    last_speed = glm::length(ac.velocity);
    last_alpha = 0.0f;
    last_CL = 0.0f;
    last_CD = 0.0f;
    last_L = 0.0f;
    last_D = 0.0f;
}
