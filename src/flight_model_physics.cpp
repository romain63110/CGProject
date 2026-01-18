#include "flight_model_physics.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

void FlightModelPhysics::step(Aircraft& ac, const AircraftControls& u, float dt)
{
    glm::vec3 forward = ac.orientation * glm::vec3(0, 0, -1);

    glm::vec3 Ft = forward * (u.throttle * Tmax);

    glm::vec3 a = Ft / mass;

    ac.velocity += a * dt;
    ac.position += ac.velocity * dt;

    last_speed = glm::length(ac.velocity);
    last_alpha = 0.0f;
    last_CL = 0.0f;
    last_CD = 0.0f;
    last_L = 0.0f;
    last_D = 0.0f;
}
