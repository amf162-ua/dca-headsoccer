#include "Stadium.hpp"

namespace
{
    // Público de Mestalla: anillo superior, anillo inferior, grada baja y laterales.
    // Coinciden con las filas de asientos de stadium_background.png.
    const Rectangle MESTALLA_CROWD_ZONES[] = {
        { 180.0f, 254.0f, 720.0f, 80.0f },   // Anillo superior
        { 150.0f, 356.0f, 780.0f, 128.0f },  // Anillo inferior
        { 0.0f, 506.0f, 1080.0f, 50.0f },    // Grada baja, junto al césped
        { 0.0f, 318.0f, 172.0f, 228.0f },    // Grada lateral izquierda
        { 908.0f, 318.0f, 172.0f, 228.0f }   // Grada lateral derecha
    };
    // Afición valencianista: sobre todo blanco, negro y naranja
    const Color MESTALLA_SHIRTS[] = {
        { 245, 245, 245, 255 }, { 245, 245, 245, 255 }, { 245, 245, 245, 255 },
        { 35, 35, 40, 255 }, { 35, 35, 40, 255 },
        { 238, 110, 40, 255 }, { 238, 110, 40, 255 },
        { 250, 205, 60, 255 }, { 70, 110, 190, 255 }, { 150, 150, 155, 255 }
    };

    template <typename T, int N>
    constexpr int lengthOf(const T (&)[N]) { return N; }

    // Orden en el que aparecen en el carrusel del menú.
    const StadiumInfo STADIUMS[] = {
        { StadiumId::Classic, "CANCHA CLASICA", "El campo de siempre. A jugar!",
          { 92, 220, 126, 255 }, true, nullptr, nullptr },
        { StadiumId::Mestalla, "MESTALLA", "El estadio del Valencia CF, desde 1923.",
          { 238, 110, 40, 255 }, true,
          "sprites/stadium_background.png", "sprites/grass.png",
          MESTALLA_CROWD_ZONES, lengthOf(MESTALLA_CROWD_ZONES),
          MESTALLA_SHIRTS, lengthOf(MESTALLA_SHIRTS) },
        { StadiumId::Lunar, "ESTADIO LUNAR", "Partido espacial sobre la superficie lunar.",
          { 180, 195, 220, 255 }, true,
          "sprites/moon_background.png", "sprites/moon_ground.png" },
        { StadiumId::NightCity, "CIUDAD NOCTURNA", "Un nuevo escenario llegara pronto.",
          { 93, 150, 255, 255 }, false, nullptr, nullptr },
        { StadiumId::TropicalBeach, "PLAYA TROPICAL", "Un nuevo escenario llegara pronto.",
          { 255, 190, 75, 255 }, false, nullptr, nullptr }
    };
    constexpr int STADIUM_COUNT = sizeof(STADIUMS) / sizeof(STADIUMS[0]);
}

namespace Stadiums
{
    int count()
    {
        return STADIUM_COUNT;
    }

    const StadiumInfo& at(int index)
    {
        if (index < 0 || index >= STADIUM_COUNT)
        {
            return STADIUMS[0];
        }
        return STADIUMS[index];
    }

    const StadiumInfo& get(StadiumId id)
    {
        for (const StadiumInfo& stadium : STADIUMS)
        {
            if (stadium.id == id)
            {
                return stadium;
            }
        }
        return STADIUMS[0];
    }
}
