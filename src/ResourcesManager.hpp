#pragma once

#include <string>

extern "C" {
    #include <raylib.h>
}

class ResourcesManager
{
public:
    static Font LoadFont(const std::string& fontFile);
    static void UnloadFont(Font font);
};
