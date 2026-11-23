#include "ResourcesManager.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

void checkPlayerTexture(Texture2D texture)
{
    assert(IsTextureValid(texture));
    assert(texture.width == 50);
    assert(texture.height == 80);

    // Comprobamos que el PNG conserva el fondo transparente al cargarlo en la GPU.
    Image image = LoadImageFromTexture(texture);
    Color* pixels = LoadImageColors(image);
    assert(pixels != nullptr);
    bool hasTransparentPixels = false;
    bool hasVisiblePixels = false;
    for (int index = 0; index < image.width * image.height; index++)
    {
        if (pixels[index].a == 0)
        {
            hasTransparentPixels = true;
        }
        if (pixels[index].a == 255)
        {
            hasVisiblePixels = true;
        }
    }
    assert(hasTransparentPixels && hasVisiblePixels);
    UnloadImageColors(pixels);
    UnloadImage(image);
}

int main()
{
    // Raylib necesita un contexto grafico para crear texturas, aunque no se vea.
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(160, 160, "Prueba de texturas");

    Texture2D player1 = ResourcesManager::LoadPlayerTexture(1);
    Texture2D player2 = ResourcesManager::LoadPlayerTexture(2);
    checkPlayerTexture(player1);
    checkPlayerTexture(player2);

    bool rejectedInvalidPlayer = false;
    try
    {
        ResourcesManager::LoadPlayerTexture(3);
    }
    catch (const std::invalid_argument&)
    {
        rejectedInvalidPlayer = true;
    }
    assert(rejectedInvalidPlayer);

    ResourcesManager::UnloadTexture(player1);
    ResourcesManager::UnloadTexture(player2);
    CloseWindow();
    std::cout << "Pruebas de texturas superadas." << std::endl;
    return 0;
}
