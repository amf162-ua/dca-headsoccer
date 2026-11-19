#include "ResourcesManager.hpp"

#include <stdexcept>

#ifndef GAME_INSTALL_DATA_DIR
#define GAME_INSTALL_DATA_DIR ""
#endif

namespace
{
    // Busca un recurso en el directorio actual, en assets/, junto al ejecutable
    // y en la ruta de instalación. "kind" solo se usa en los mensajes de error.
    std::string resolveAssetPath(const std::string& assetFile, const std::string& kind)
    {
        if (assetFile.empty())
        {
            throw std::invalid_argument("La ruta de " + kind + " no puede estar vacía");
        }

        if (FileExists(assetFile.c_str()))
        {
            return assetFile;
        }

        const std::string assetRelativePath = "assets/" + assetFile;
        if (FileExists(assetRelativePath.c_str()))
        {
            return assetRelativePath;
        }

        const std::string applicationPath = std::string(GetApplicationDirectory()) +
                                            assetRelativePath;
        if (FileExists(applicationPath.c_str()))
        {
            return applicationPath;
        }

        const std::string installPath = std::string(GAME_INSTALL_DATA_DIR) +
                                        "/" + assetRelativePath;
        if (FileExists(installPath.c_str()))
        {
            return installPath;
        }

        throw std::runtime_error("No se pudo encontrar " + kind + ": " + assetFile);
    }
}

Font ResourcesManager::LoadFont(const std::string& fontFile)
{
    const std::string resolvedPath = resolveAssetPath(fontFile, "la fuente");
    Font font = ::LoadFont(resolvedPath.c_str());
    if (!IsFontValid(font))
    {
        throw std::runtime_error("Raylib no pudo cargar la fuente: " + resolvedPath);
    }

    return font;
}

void ResourcesManager::UnloadFont(Font font)
{
    if (IsFontValid(font))
    {
        ::UnloadFont(font);
    }
}

Texture2D ResourcesManager::LoadTexture(const std::string& textureFile)
{
    const std::string resolvedPath = resolveAssetPath(textureFile, "la textura");
    Texture2D texture = ::LoadTexture(resolvedPath.c_str());
    if (!IsTextureValid(texture))
    {
        throw std::runtime_error("Raylib no pudo cargar la textura: " + resolvedPath);
    }

    return texture;
}

void ResourcesManager::UnloadTexture(Texture2D texture)
{
    if (IsTextureValid(texture))
    {
        ::UnloadTexture(texture);
    }
}
