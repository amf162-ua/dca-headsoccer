#include "Crowd.hpp"
#include "ResourcesManager.hpp"

#include <cmath>
#include <random>

namespace
{
    // Medidas del sprite sheet (crowd_base.png y crowd_shirt.png): 4 fotogramas de 8x12 px
    constexpr float FRAME_WIDTH = 8.0f;
    constexpr float FRAME_HEIGHT = 12.0f;
    constexpr int FRAME_SITTING = 0;
    constexpr int FRAME_CHEERING = 1;
    constexpr int FRAME_STANDING = 2;
    constexpr int FRAME_ARMS_UP = 3;

    // Reparto en la grada
    constexpr float ROW_SPACING = 12.0f;   // Separación vertical entre filas de público
    constexpr float MIN_SEAT_GAP = 7.0f;   // Separación horizontal mínima entre aficionados
    constexpr float MAX_SEAT_GAP = 13.0f;
    constexpr int OCCUPANCY_PERCENT = 55;  // Asientos ocupados (deja ver el mosaico de la grada)

    // La ola
    constexpr float WAVE_SPEED = 520.0f;   // px/s
    constexpr float WAVE_WIDTH = 70.0f;    // Ancho de la zona que se levanta
    constexpr float WAVE_INTERVAL = 14.0f; // Segundos entre olas

    // Tinte general: el público se ve algo apagado para no distraer del juego
    constexpr Color CROWD_TINT = { 225, 225, 230, 235 };

    float randomRange(std::mt19937& rng, float min, float max)
    {
        return std::uniform_real_distribution<float>(min, max)(rng);
    }

    Color multiply(Color a, Color b)
    {
        return {
            static_cast<unsigned char>(a.r * b.r / 255),
            static_cast<unsigned char>(a.g * b.g / 255),
            static_cast<unsigned char>(a.b * b.b / 255),
            static_cast<unsigned char>(a.a * b.a / 255)
        };
    }
}

Crowd::~Crowd()
{
    ResourcesManager::UnloadTexture(baseTexture);
    ResourcesManager::UnloadTexture(shirtTexture);
}

void Crowd::init(const Rectangle* zones, int zoneCount, const Color* shirtColors, int colorCount)
{
    spectators.clear();
    if (zones == nullptr || zoneCount <= 0 || shirtColors == nullptr || colorCount <= 0)
    {
        return;
    }

    baseTexture = ResourcesManager::LoadTexture("sprites/crowd_base.png");
    shirtTexture = ResourcesManager::LoadTexture("sprites/crowd_shirt.png");

    // Generador propio con semilla fija: el público aparece siempre igual
    // y no altera el rand() global que pueda usar el resto del juego.
    std::mt19937 rng(1919);
    std::uniform_int_distribution<int> percent(0, 99);
    std::uniform_int_distribution<int> colorIndex(0, colorCount - 1);

    // Recorremos cada zona fila a fila, de atrás (arriba) hacia delante (abajo),
    // para que los de delante se dibujen encima de los de detrás.
    for (int zone = 0; zone < zoneCount; ++zone)
    {
        const Rectangle& area = zones[zone];
        for (float rowY = area.y; rowY + FRAME_HEIGHT <= area.y + area.height; rowY += ROW_SPACING)
        {
            float x = area.x + randomRange(rng, 0.0f, MAX_SEAT_GAP);
            while (x + FRAME_WIDTH <= area.x + area.width)
            {
                if (percent(rng) < OCCUPANCY_PERCENT)
                {
                    Spectator spectator;
                    spectator.position = { std::floor(x), rowY };
                    spectator.shirt = shirtColors[colorIndex(rng)];
                    spectator.phase = randomRange(rng, 0.0f, 6.2832f);
                    spectator.excitement = randomRange(rng, 0.3f, 1.0f);
                    spectators.push_back(spectator);
                }
                x += randomRange(rng, MIN_SEAT_GAP, MAX_SEAT_GAP);
            }
        }
    }

    time = 0.0f;
    waveTimer = WAVE_INTERVAL * 0.5f; // La primera ola llega antes
    waveX = -1.0f;
}

void Crowd::update(float deltaTime, bool isCelebrating)
{
    if (spectators.empty())
    {
        return;
    }

    time += deltaTime;
    celebrationTime = isCelebrating ? celebrationTime + deltaTime : 0.0f;
    celebrating = isCelebrating;

    // La ola recorre el estadio de izquierda a derecha cada cierto tiempo
    if (waveX >= 0.0f)
    {
        waveX += WAVE_SPEED * deltaTime;
        if (waveX > GetScreenWidth() + WAVE_WIDTH)
        {
            waveX = -1.0f;
            waveTimer = WAVE_INTERVAL;
        }
    }
    else
    {
        waveTimer -= deltaTime;
        if (waveTimer <= 0.0f)
        {
            waveX = 0.0f;
        }
    }
}

// Elige el fotograma de un aficionado y cuánto sube o baja este fotograma.
int Crowd::frameFor(const Spectator& spectator, float& offsetY) const
{
    offsetY = 0.0f;

    // Gol: todos de pie con los brazos arriba, saltando cada uno a su ritmo
    if (celebrating)
    {
        const float jump = std::fabs(std::sin(celebrationTime * 9.0f + spectator.phase));
        offsetY = -std::floor(jump * 3.0f * spectator.excitement);
        return FRAME_ARMS_UP;
    }

    // La ola: quien está bajo ella se levanta y alza los brazos
    if (waveX >= 0.0f)
    {
        const float distance = std::fabs(spectator.position.x - waveX);
        if (distance < WAVE_WIDTH * 0.5f)
        {
            return FRAME_ARMS_UP;
        }
        if (distance < WAVE_WIDTH)
        {
            return FRAME_STANDING;
        }
    }

    // Reposo: balanceo suave y, de vez en cuando, alguno anima con el brazo
    const float sway = std::sin(time * 2.2f + spectator.phase);
    offsetY = (sway > 0.6f) ? -1.0f : 0.0f;
    const float cheer = std::sin(time * 0.7f + spectator.phase * 3.0f);
    return (cheer > 0.93f && spectator.excitement > 0.6f) ? FRAME_CHEERING : FRAME_SITTING;
}

void Crowd::render() const
{
    if (spectators.empty())
    {
        return;
    }

    for (const Spectator& spectator : spectators)
    {
        float offsetY = 0.0f;
        const int frame = frameFor(spectator, offsetY);
        const Rectangle source = { frame * FRAME_WIDTH, 0.0f, FRAME_WIDTH, FRAME_HEIGHT };
        const Vector2 position = { spectator.position.x, spectator.position.y + offsetY };

        DrawTextureRec(baseTexture, source, position, CROWD_TINT);
        DrawTextureRec(shirtTexture, source, position, multiply(spectator.shirt, CROWD_TINT));
    }
}
