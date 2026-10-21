#include "physics.hpp"
#include <iostream>

Physics::Physics()
{
}

/**
    Aplica la gravedad al objeto.
    vy: velocidad en y (px/s)
    deltaTime: tiempo transcurrido (s)
*/
void Physics::Gravity(float& vy, float deltaTime)
{
    // En px/s^2, no m/s^2. Es 9.81 * 60: lo que antes se sumaba por frame (a 60 FPS)
    // pasado a por segundo, para que el juego se sienta igual que antes.
    const float gravityAcceleration = 588.6f;
    vy += gravityAcceleration * deltaTime;
}
/*
    Friccion aplicada.
    vx: velocidad en x (px/s)
    frictionCoefficient: coeficiente de friccion (px/s^2, cuanta velocidad se pierde por segundo)
    deltaTime: tiempo transcurrido (s)
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

/*
    Mueve el cuerpo segun su velocidad (integracion de Euler).
    body: cuerpo a mover (su posicion es el centro)
    deltaTime: tiempo transcurrido (s)
*/
void Physics::Integrate(Body& body, float deltaTime)
{
    body.getPosition().x += body.getVelocity().x * deltaTime;
    body.getPosition().y += body.getVelocity().y * deltaTime;
}
