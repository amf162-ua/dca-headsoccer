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

    static Texture2D LoadTexture(const std::string& textureFile);
    static void UnloadTexture(Texture2D texture);
};
