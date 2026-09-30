#pragma once
extern "C" {
    #include <raylib.h>
}

class Physics
{
    public:
        Physics();
        ~Physics() = default;
        void Gravity(float& vy, float deltaTime);
        void ApplyFriction(float& vx, float frictionCoefficient, float deltaTime);
};