#pragma once

extern "C" {
    #include <raylib.h>
}

// Identificador de cada estadio (cancha) que se puede elegir en el menú.
enum class StadiumId
{
    Classic,
    Mestalla,
    Lunar,
    NightCity,
    TropicalBeach
};

// Datos de un estadio. Para añadir uno nuevo basta con añadir su StadiumId
// y una entrada en la lista de Stadium.cpp con sus sprites.
struct StadiumInfo
{
    StadiumId id;
    const char* name;
    const char* description;
    Color accent;
    bool available;
    const char* backgroundSprite; // Fondo 1080x600 (nullptr = fondo liso clásico)
    const char* grassSprite;      // Césped 1080x120 (nullptr = césped liso clásico)
};

namespace Stadiums
{
    int count();
    const StadiumInfo& at(int index);  // Por posición en el carrusel
    const StadiumInfo& get(StadiumId id);
}
