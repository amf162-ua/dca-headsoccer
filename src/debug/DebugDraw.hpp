#pragma once
#include <raylib.h>

namespace DebugDraw {

void box(Rectangle bounds, Color color, bool filled = false);

void arrow(Vector2 origin, Vector2 value, float scale, Color color);

}