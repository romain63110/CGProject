#pragma once

struct AircraftControls
{
    float elevator = 0.0f;
    float aileron = 0.0f;
    float throttle = 0.0f;

    void resetAxis();
};
