#include "DebugDraw.hpp"

#include <cmath>

namespace DebugDraw
{
    void box(Rectangle bounds, Color color)
    {
        DrawRectangleLinesEx(bounds, 2.0f, color);
    }

    void arrow(Vector2 origin, Vector2 value, float scale, Color color)
    {
        Vector2 scaledValue = {value.x * scale, value.y * scale};
        float arrowLength = std::sqrt(
            scaledValue.x * scaledValue.x +
            scaledValue.y * scaledValue.y
        );

        DrawCircleV(origin, 3.0f, color);

        if (arrowLength == 0.0f)
        {
            return;
        }

        Vector2 direction = {
            scaledValue.x / arrowLength,
            scaledValue.y / arrowLength
        };
        Vector2 end = {
            origin.x + scaledValue.x,
            origin.y + scaledValue.y
        };
        Vector2 perpendicular = {-direction.y, direction.x};

        float headSize = 8.0f;
        if (arrowLength < 16.0f)
        {
            headSize = arrowLength / 2.0f;
        }

        Vector2 leftWing = {
            end.x - direction.x * headSize + perpendicular.x * headSize / 2.0f,
            end.y - direction.y * headSize + perpendicular.y * headSize / 2.0f
        };
        Vector2 rightWing = {
            end.x - direction.x * headSize - perpendicular.x * headSize / 2.0f,
            end.y - direction.y * headSize - perpendicular.y * headSize / 2.0f
        };

        DrawLineEx(origin, end, 2.0f, color);
        DrawLineEx(end, leftWing, 2.0f, color);
        DrawLineEx(end, rightWing, 2.0f, color);
    }
}
