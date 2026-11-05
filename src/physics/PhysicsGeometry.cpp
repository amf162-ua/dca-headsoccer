#include "PhysicsGeometry.hpp"

namespace PhysicsGeometry
{
    Rectangle playerBounds(float x, float y)
    {
        Rectangle bounds = {x, y, playerWidth, playerHeight};
        return bounds;
    }

    Rectangle ballBounds(float x, float y)
    {
        Rectangle bounds = {
            x - ballRadius,
            y - ballRadius,
            ballRadius * 2.0f,
            ballRadius * 2.0f
        };
        return bounds;
    }

    Vector2 center(Rectangle rectangle)
    {
        Vector2 point = {
            rectangle.x + rectangle.width / 2.0f,
            rectangle.y + rectangle.height / 2.0f
        };
        return point;
    }
}
