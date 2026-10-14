#pragma once
#include <raylib.h>

namespace PhysicsGeometry {
const float playerWidth = 50.0f;
const float playerHeight = 80.0f;
const float ballRadius = 20.0f;
const float groundY = 600.0f;
const float fieldWidth = 1080.0f;

inline Rectangle playerBounds(float x, float y) {
    return {x, y, playerWidth, playerHeight};
}

inline Rectangle ballBounds(float x, float y) {
    return {
        x - ballRadius,
        y - ballRadius,
        2.0f * ballRadius, 
        2.0f * ballRadius
    };
}

inline Vector2 center(Rectangle r) {
    return {
        r.x + r.width / 2.0f,
        r.y + r.height / 2.0f
    };
}

}


