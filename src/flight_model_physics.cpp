#include "flight_model_physics.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cmath>

const FlightModelPhysics::CurvePoint FlightModelPhysics::liftCurve[] =
{
    { glm::radians(-90.0f), -1.0f },
    { glm::radians(-30.0f), -1.0f },
    { glm::radians(0.0f),   0.0f },
    { glm::radians(30.0f),  1.0f },
    { glm::radians(90.0f),  1.0f }
};

const int FlightModelPhysics::liftCurveCount =
sizeof(FlightModelPhysics::liftCurve) / sizeof(FlightModelPhysics::liftCurve[0]);

float FlightModelPhysics::evalCurve(const CurvePoint* pts, int count, float x) const
{
    if (count <= 0) return 0.0f;
    if (x <= pts[0].x) return pts[0].y;
    if (x >= pts[count - 1].x) return pts[count - 1].y;

    for (int i = 0; i < count - 1; i++)
    {
        float x0 = pts[i].x;
        float x1 = pts[i + 1].x;

        if (x >= x0 && x <= x1)
        {
            float t = (x - x0) / (x1 - x0);
            return pts[i].y + t * (pts[i + 1].y - pts[i].y);
        }
    }

    return pts[count - 1].y;
}

void FlightModelPhysics::step(Aircraft& ac, const AircraftControls& u, float dt)
{
    glm::vec3 forward = ac.orientation * glm::vec3(0, 0, -1);
    glm::vec3 right = ac.orientation * glm::vec3(1, 0, 0);

    float roll_rate = glm::radians(roll_rate_deg);
    float pitch_rate = glm::radians(pitch_rate_deg);

    glm::quat qRoll = glm::angleAxis(roll_rate * u.aileron * dt, forward);
    glm::quat qPitch = glm::angleAxis(pitch_rate * u.elevator * dt, right);

    ac.orientation = glm::normalize(qPitch * qRoll * ac.orientation);

    forward = ac.orientation * glm::vec3(0, 0, -1);
    glm::vec3 Ft = forward * (u.throttle * Tmax);

    glm::vec3 v_world = ac.velocity;
    float speed = glm::length(v_world);

    glm::vec3 Fd_world(0.0f);
    if (speed > 0.001f)
    {
        glm::vec3 v_local = glm::inverse(ac.orientation) * v_world;
        dbg_v_local = v_local;

        float kx = (v_local.x >= 0.0f) ? dragRight : dragLeft;
        float ky = (v_local.y >= 0.0f) ? dragUp : dragDown;
        float kz = (v_local.z <= 0.0f) ? dragForward : dragBack;

        glm::vec3 Fd_local;
        Fd_local.x = -kx * v_local.x * std::abs(v_local.x);
        Fd_local.y = -ky * v_local.y * std::abs(v_local.y);
        Fd_local.z = -kz * v_local.z * std::abs(v_local.z);

        Fd_world = ac.orientation * Fd_local;
    }

    glm::vec3 FL(0.0f);

    if (speed > 0.001f)
    {
        glm::vec3 v_local = glm::inverse(ac.orientation) * v_world;

        float vz = -v_local.z;
        float vy = v_local.y;

        float alpha = std::atan2(-vy, vz);

        float CLmax = 1.2f;
        float CL = CLmax * std::sin(2.0f * alpha);

        

        float vMin = 1.0f;
        if (std::abs(vz) < vMin) CL = 0.0f;

        float L = 0.5f * CL * liftPower * vz * vz;

        glm::vec3 up = ac.orientation * glm::vec3(0, 1, 0);
        FL = up * L;

        alpha_deg = glm::degrees(alpha);
        last_alpha = alpha;
        last_CL = CL;
        last_L = L;
    }

    glm::vec3 Fg(0.0f, -mass * 9.81f, 0.0f);

    

    glm::vec3 a = (Ft + Fd_world + Fg + FL ) / mass;

    ac.velocity += a * dt;
    ac.position += ac.velocity * dt;

    float runwayLength = 160000.0f;
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
    }

    last_speed = glm::length(ac.velocity);

    dbg_Fg = Fg;
    dbg_Fd = Fd_world;
    dbg_FL = FL;
    dbg_Ft = Ft;
    dbg_Fsum = Ft + FL + Fd_world + Fg;
    dbg_v_world = v_world;

    
}
