#ifndef PHYSICS_GEOMETRY_HPP
#define PHYSICS_GEOMETRY_HPP

#include <raylib.h>

namespace PhysicsGeometry
{
    const float playerWidth = 50.0f;
    const float playerHeight = 80.0f;
    const float ballRadius = 20.0f;
    const float groundY = 600.0f;
    const float fieldWidth = 1080.0f;

    Rectangle playerBounds(float x, float y);
    Rectangle ballBounds(float x, float y);
    Vector2 center(Rectangle rectangle);
}

#endif
