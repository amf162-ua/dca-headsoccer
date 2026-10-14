#pragma once
#include "body.hpp"


namespace Collision
{
    struct Contact{
        bool isColliding = false;    // Indica si hay colisión
        Vector2 normal = { 0, 0 };   // Vector normal de la colisión Rectangulo -> Circulo (longitud 1)
        float penetration = 0.0f;    // Profundidad de penetración
    };
    Contact detectCircleAABB(const Body& circle, const Body& rectangle);
    void resolveContact(Body& circle, const Body& rectangle, const Contact& contact, float e); // e significa restitución (Esta en el png bueno de ideas, MATEEESSss)
}
