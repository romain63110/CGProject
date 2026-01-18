#pragma once
#include "aircraft.h"
#include "aircraft_controls.h"

class FlightModelPhysics
{
public:
    float mass = 1200.0f;
    float Tmax = 15000.0f;

    float pitch_rate_deg = 80.0f;
    float roll_rate_deg = 170.0f;

    float drag_k = 0.25f;

    void step(Aircraft& ac, const AircraftControls& u, float dt);

    float last_alpha = 0.0f;
    float last_speed = 0.0f;
    float last_CL = 0.0f;
    float last_CD = 0.0f;
    float last_L = 0.0f;
    float last_D = 0.0f;
};
