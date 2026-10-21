#ifndef DEBUG_DRAW_HPP
#define DEBUG_DRAW_HPP

#include <raylib.h>

namespace DebugDraw
{
    void box(Rectangle bounds, Color color);
    void arrow(Vector2 origin, Vector2 value, float scale, Color color);
}

#endif
