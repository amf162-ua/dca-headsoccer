#include "ResourcesManager.hpp"

#include <stdexcept>

#ifndef GAME_INSTALL_DATA_DIR
#define GAME_INSTALL_DATA_DIR ""
#endif

namespace
{
    std::string resolveFontPath(const std::string& fontFile)
    {
        if (fontFile.empty())
        {
            throw std::invalid_argument("La ruta de la fuente no puede estar vacía");
        }

        if (FileExists(fontFile.c_str()))
        {
            return fontFile;
        }

        const std::string assetRelativePath = "assets/" + fontFile;
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

        throw std::runtime_error("No se pudo encontrar la fuente: " + fontFile);
    }
}

Font ResourcesManager::LoadFont(const std::string& fontFile)
{
    const std::string resolvedPath = resolveFontPath(fontFile);
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
