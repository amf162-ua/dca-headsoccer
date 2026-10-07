#include "DebugDraw.hpp"
#include <algorithm>
#include <cmath>

void DebugDraw::box(Rectangle bounds, Color color, bool filled) {
    if (filled) {
        DrawRectangleRec(bounds, Fade(color, 0.15f));
    }

    DrawRectangleLinesEx(bounds, 2.0f, color);
}

void DebugDraw::arrow(
    Vector2 origin,
    Vector2 value,
    float scale,
    Color color
) {
    Vector2 delta = {value.x * scale, value.y * scale};
    float length = std::sqrt(delta.x * delta.x + delta.y * delta.y);

    DrawCircleV(origin, 3.0f, color);

    // Evita normalizar un vector nulo y dibujar flechas diminutas.
    if (length < 1.0f) return;

    Vector2 end = {origin.x + delta.x, origin.y + delta.y};
    Vector2 direction = {delta.x / length, delta.y / length};
    Vector2 perpendicular = {-direction.y, direction.x};

    float head = std::min(8.0f, length * 0.5f);

    Vector2 left = {
        end.x - direction.x * head + perpendicular.x * head * 0.5f,
        end.y - direction.y * head + perpendicular.y * head * 0.5f
    };

    Vector2 right = {
        end.x - direction.x * head - perpendicular.x * head * 0.5f,
        end.y - direction.y * head - perpendicular.y * head * 0.5f
    };

    DrawLineEx(origin, end, 2.0f, color);
    DrawLineEx(end, left, 2.0f, color);
    DrawLineEx(end, right, 2.0f, color);
}