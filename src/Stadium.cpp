#include "Stadium.hpp"

namespace
{
    // Orden en el que aparecen en el carrusel del menú.
    const StadiumInfo STADIUMS[] = {
        { StadiumId::Classic, "CANCHA CLASICA", "El campo de siempre. A jugar!",
          { 92, 220, 126, 255 }, true, nullptr, nullptr },
        { StadiumId::Mestalla, "MESTALLA", "El estadio del Valencia CF, desde 1923.",
          { 238, 110, 40, 255 }, true,
          "sprites/stadium_background.png", "sprites/grass.png" },
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
