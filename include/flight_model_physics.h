#pragma once
#include "aircraft.h"
#include "aircraft_controls.h"
#include <glm/glm.hpp>

class FlightModelPhysics
{
public:
    float mass = 1200.0f;
    float Tmax = 15000.0f;

    float roll_rate_deg = 170.0f;
    float pitch_rate_deg = 90.0f;

    float roll_accel_deg = 600.0f;
    float pitch_accel_deg = 450.0f;

    float steerSpeedRef = 50.0f;

    float stallAngleDeg = 15.0f;
    float stallNoseDrop = 2.0f;

    void step(Aircraft& ac, const AircraftControls& u, float dt);

    glm::vec3 dbg_v_world{ 0.0f };
    glm::vec3 dbg_v_local{ 0.0f };

    glm::vec3 dbg_Ft{ 0.0f };
    glm::vec3 dbg_Fd{ 0.0f };
    glm::vec3 dbg_FL{ 0.0f };
    glm::vec3 dbg_Fg{ 0.0f };
    glm::vec3 dbg_Fsum{ 0.0f };

    float liftPower = 1.5f;

    float dragRight = 1.0f;
    float dragLeft = 1.0f;
    float dragUp = 1.0f;
    float dragDown = 1.0f;
    float dragForward = 0.2f;
    float dragBack = 3.0f;

    float rho = 1.225f;
    float wingArea = 16.0f;

    float alpha_deg = 0.0f;

    float CLmax = 1.2f;
    float inducedDrag = 0.05f;

    float last_alpha = 0.0f;
    float last_speed = 0.0f;
    float last_CL = 0.0f;
    float last_CD = 0.0f;
    float last_L = 0.0f;
    float last_D = 0.0f;

private:
    struct CurvePoint { float x; float y; };
    static const CurvePoint liftCurve[];
    static const int liftCurveCount;

    float evalCurve(const CurvePoint* pts, int count, float x) const;

    static float clampf(float v, float lo, float hi)
    {
        return (v < lo) ? lo : (v > hi) ? hi : v;
    }
};
