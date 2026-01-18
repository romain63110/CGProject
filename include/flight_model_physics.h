#pragma once
#include "aircraft.h"
#include "aircraft_controls.h"

class FlightModelPhysics
{
public:
    float mass = 1200.0f;

    float wing_area = 16.0f;
    float rho = 1.225f;
    float g = 9.81f;

    float CL0 = 0.2f;
    float CL_alpha = 5.5f;
    float CL_max = 1.4f;

    float CD0 = 0.02f;
    float k = 0.04f;

    float Tmax = 15000.0f;

    float pitch_power = 2500.0f;
    float roll_power = 1800.0f;
    float ang_damping = 120.0f;

    glm::vec3 inertia = glm::vec3(2500.0f, 4500.0f, 3000.0f);

    void step(Aircraft& ac, const AircraftControls& u, float dt);

    float last_alpha = 0.0f;
    float last_speed = 0.0f;
    float last_CL = 0.0f;
    float last_CD = 0.0f;
    float last_L = 0.0f;
    float last_D = 0.0f;
};
