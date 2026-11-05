#pragma once
extern "C" {
    #include <raylib.h>
}
#include "body.hpp"

class Physics
{
    public:
        Physics();
        ~Physics() = default;
        void Gravity(float& vy, float deltaTime);
        void ApplyFriction(float& vx, float frictionCoefficient, float deltaTime);
        void Integrate(Body& body, float deltaTime); // Mueve el cuerpo segun su velocidad: pos += v * dt
};