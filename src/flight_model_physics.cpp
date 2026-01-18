#include "flight_model_physics.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

void FlightModelPhysics::step(Aircraft& ac, const AircraftControls& u, float dt)
{
    glm::vec3 forward = ac.orientation * glm::vec3(0, 0, -1);
    glm::vec3 right = ac.orientation * glm::vec3(1, 0, 0);

    float pitch_rate = glm::radians(pitch_rate_deg);
    float roll_rate = glm::radians(roll_rate_deg);

    glm::quat qPitch = glm::angleAxis(pitch_rate * u.elevator * dt, right);
    glm::quat qRoll = glm::angleAxis(roll_rate * u.aileron * dt, forward);

    ac.orientation = glm::normalize(qRoll * qPitch * ac.orientation);

    forward = ac.orientation * glm::vec3(0, 0, -1);

    glm::vec3 Ft = forward * (u.throttle * Tmax);

    glm::vec3 v = ac.velocity;
    float speed = glm::length(v);

    glm::vec3 Fd = glm::vec3(0.0f);
    if (speed > 0.001f)
        Fd = -drag_k * v * speed;

    glm::vec3 a = (Ft + Fd) / mass;

    ac.velocity += a * dt;
    ac.position += ac.velocity * dt;

    last_speed = glm::length(ac.velocity);
    last_alpha = 0.0f;
    last_CL = 0.0f;
    last_CD = 0.0f;
    last_L = 0.0f;
    last_D = 0.0f;
}
