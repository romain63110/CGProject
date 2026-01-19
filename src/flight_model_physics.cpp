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

static float clampStep(float dt, float current, float target, float accel)
{
    float error = target - current;
    float maxDelta = accel * dt;
    if (error > maxDelta) error = maxDelta;
    if (error < -maxDelta) error = -maxDelta;
    return error;
}

void FlightModelPhysics::step(Aircraft& ac, const AircraftControls& u, float dt)
{
    if (dt <= 0.0f) return;

    glm::vec3 v_world = ac.velocity;
    float speed = glm::length(v_world);

    glm::vec3 v_local(0.0f);
    if (speed > 0.001f)
        v_local = glm::inverse(ac.orientation) * v_world;

    dbg_v_local = v_local;

    float speedTotal = glm::length(v_local);
    float steeringPower = clampf(speedTotal / steerSpeedRef, 0.0f, 1.0f);

    float rollRate = glm::radians(roll_rate_deg) * steeringPower;
    float pitchRate = glm::radians(pitch_rate_deg) * steeringPower;

    float rollAccel = glm::radians(roll_accel_deg) * steeringPower;
    float pitchAccel = glm::radians(pitch_accel_deg) * steeringPower;

    float vz = -v_local.z;
    float vy = v_local.y;

    float vzMin = 3.0f;
    float alpha = last_alpha;
    if (speed > 0.001f && std::abs(vz) > vzMin)
        alpha = std::atan2(-vy, vz);

    float stallAngle = glm::radians(stallAngleDeg);

    glm::vec3 targetOmegaBody(0.0f);
    targetOmegaBody.x = u.elevator * pitchRate;
    targetOmegaBody.z = -u.aileron * rollRate;

    if (std::abs(alpha) > stallAngle)
    {
        float extra = (std::abs(alpha) - stallAngle);
        float s = (alpha >= 0.0f) ? 1.0f : -1.0f;
        targetOmegaBody.x -= s * (stallNoseDrop * extra);
    }

    ac.omega_body.x += clampStep(dt, ac.omega_body.x, targetOmegaBody.x, pitchAccel);
    ac.omega_body.z += clampStep(dt, ac.omega_body.z, targetOmegaBody.z, rollAccel);

    glm::vec3 omega_world = ac.orientation * ac.omega_body;
    float omegaMag = glm::length(omega_world);
    if (omegaMag > 1e-6f)
    {
        glm::vec3 axis = omega_world / omegaMag;
        float angle = omegaMag * dt;
        glm::quat dq = glm::angleAxis(angle, axis);
        ac.orientation = glm::normalize(dq * ac.orientation);
    }

    glm::vec3 forward = ac.orientation * glm::vec3(0, 0, -1);
    glm::vec3 Ft = forward * (u.throttle * Tmax);

    v_world = ac.velocity;
    speed = glm::length(v_world);

    glm::vec3 Fd_world(0.0f);
    if (speed > 0.001f)
    {
        glm::vec3 vloc = glm::inverse(ac.orientation) * v_world;

        float kx = (vloc.x >= 0.0f) ? dragRight : dragLeft;
        float ky = (vloc.y >= 0.0f) ? dragUp : dragDown;
        float kz = (vloc.z <= 0.0f) ? dragForward : dragBack;

        glm::vec3 Fd_local;
        Fd_local.x = -kx * vloc.x * std::abs(vloc.x);
        Fd_local.y = -ky * vloc.y * std::abs(vloc.y);
        Fd_local.z = -kz * vloc.z * std::abs(vloc.z);

        Fd_world = ac.orientation * Fd_local;
    }

    glm::vec3 FL(0.0f);
    glm::vec3 FDi(0.0f);

    if (speed > 0.001f)
    {
        glm::vec3 vloc = glm::inverse(ac.orientation) * v_world;

        float vz2 = -vloc.z;
        float vy2 = vloc.y;

        float alpha2 = last_alpha;
        if (std::abs(vz2) > vzMin)
            alpha2 = std::atan2(-vy2, vz2);

        float CL = CLmax * std::sin(2.0f * alpha2);

        float vMin = 1.0f;
        if (std::abs(vz2) < vMin) CL = 0.0f;

        float v2 = vz2 * vz2 + vy2 * vy2;
        float liftForce = v2 * CL * liftPower;

        glm::vec3 liftVelLocal(0.0f, vy2, -vz2);
        if (glm::dot(liftVelLocal, liftVelLocal) > 1e-6f)
            liftVelLocal = glm::normalize(liftVelLocal);

        glm::vec3 rightAxisLocal(1.0f, 0.0f, 0.0f);
        glm::vec3 liftDirLocal = glm::cross(rightAxisLocal, liftVelLocal);
        if (glm::dot(liftDirLocal, liftDirLocal) > 1e-6f)
            liftDirLocal = glm::normalize(liftDirLocal);

        glm::vec3 liftDirWorld = ac.orientation * liftDirLocal;
        FL = liftDirWorld * liftForce;

        float cdi = (CL * CL) * inducedDrag;

        glm::vec3 dragDirWorld(0.0f);
        if (glm::dot(v_world, v_world) > 1e-6f)
            dragDirWorld = -glm::normalize(v_world);

        FDi = dragDirWorld * (v2 * cdi);

        alpha_deg = glm::degrees(alpha2);
        last_alpha = alpha2;
        last_CL = CL;
        last_L = liftForce;
        last_D = glm::length(FDi);
    }

    glm::vec3 Fg(0.0f, -mass * 9.81f, 0.0f);

    glm::vec3 Fsum = Ft + Fd_world + Fg + FL + FDi;
    glm::vec3 a = Fsum / mass;

    ac.velocity += a * dt;
    ac.position += ac.velocity * dt;

    float runwayLength = 4000.0f;
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
    dbg_Fsum = Fsum;
    dbg_v_world = v_world;
}
