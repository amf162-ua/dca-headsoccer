#include "physics.hpp"
#include <iostream>

Physics::Physics()
{
}

void Physics::Gravity(float& vy, float deltaTime)
{
    const float gravityAcceleration = 9.81f;
    vy += gravityAcceleration * deltaTime;
}

void Physics::ApplyFriction(float& vx, float frictionCoefficient, float deltaTime)
{
    if (vx > 0.0f)
    {
        vx -= frictionCoefficient * deltaTime;
        if (vx < 0.0f)
            vx = 0.0f;
    }
    else if (vx < 0.0f)
    {
        vx += frictionCoefficient * deltaTime;
        if (vx > 0.0f)
            vx = 0.0f;
    }
}