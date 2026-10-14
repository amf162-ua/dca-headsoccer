#include "physics.hpp"
#include <iostream>

Physics::Physics()
{
}

/**
    Aplica la gravedad al objeto.
    vy: velocidad en y
    deltaTime: tiempo transcurrido
*/
void Physics::Gravity(float& vy, float deltaTime)
{
    const float gravityAcceleration = 9.81f;
    vy += gravityAcceleration * deltaTime;
}
/*
    Friccion aplicada.
    vx: velocidad en x
    frictionCoefficient: coeficiente de friccion
    deltaTime: tiempo transcurrido
*/
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