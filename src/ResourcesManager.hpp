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
    // Cada jugador tiene su propio sprite. Quien lo carga debe liberarlo al salir.
    static Texture2D LoadPlayerTexture(int playerNumber);
    static void UnloadTexture(Texture2D texture);
};
