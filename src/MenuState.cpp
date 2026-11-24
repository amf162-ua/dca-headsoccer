#include <MenuState.hpp>
#include <MainGameState.hpp>
#include <StateMachine.hpp>
#include <ResourcesManager.hpp>
#include <Stadium.hpp>
#include <algorithm>
#include <cmath>
#include <memory>

namespace
{
    // Los estadios (nombre, descripción, color, disponibilidad y sprites) están en Stadium.cpp
    const int MAP_COUNT = Stadiums::count();
    constexpr float SCREEN_CENTER_X = 540.0f;
    constexpr Rectangle START_BUTTON = { 390.0f, 558.0f, 300.0f, 58.0f };
    constexpr Rectangle LEFT_BUTTON = { 20.0f, 311.0f, 76.0f, 82.0f };
    constexpr Rectangle RIGHT_BUTTON = { 984.0f, 311.0f, 76.0f, 82.0f };

    Rectangle getCardBounds(int mapIndex, float position)
    {
        const float distance = static_cast<float>(mapIndex) - position;
        const float scale = 1.0f - std::min(std::abs(distance) * 0.28f, 0.34f);
        const float width = 420.0f * scale;
        const float height = 278.0f * scale;

        return {
            SCREEN_CENTER_X + distance * 340.0f - width / 2.0f,
            218.0f + (1.0f - scale) * 45.0f,
            width,
            height
        };
    }

}

MenuState::MenuState()
    : selectedMap(0), carouselPosition(0.0f)
{
}

MenuState::~MenuState()
{
    for (Texture2D texture : previewBackgrounds)
    {
        ResourcesManager::UnloadTexture(texture);
    }
    for (Texture2D texture : previewGrass)
    {
        ResourcesManager::UnloadTexture(texture);
    }
}

void MenuState::init()
{
    // Cargamos una vez las miniaturas de los estadios que tienen sprites propios
    previewBackgrounds.assign(MAP_COUNT, Texture2D{});
    previewGrass.assign(MAP_COUNT, Texture2D{});
    for (int mapIndex = 0; mapIndex < MAP_COUNT; ++mapIndex)
    {
        const StadiumInfo& stadium = Stadiums::at(mapIndex);
        if (stadium.backgroundSprite != nullptr)
        {
            previewBackgrounds[mapIndex] = ResourcesManager::LoadTexture(stadium.backgroundSprite);
        }
        if (stadium.grassSprite != nullptr)
        {
            previewGrass[mapIndex] = ResourcesManager::LoadTexture(stadium.grassSprite);
        }
    }
}

void MenuState::drawMapPreview(Rectangle area, int mapIndex) const
{
        const StadiumId id = Stadiums::at(mapIndex).id;

        // Estadios con sprites: la miniatura es el propio fondo del estadio
        if (IsTextureValid(previewBackgrounds[mapIndex]))
        {
            const Texture2D background = previewBackgrounds[mapIndex];
            const float grassHeight = area.height * 0.18f;
            // Recortamos parte del cielo para que la grada se vea más grande en la tarjeta
            const float skyCut = background.height * 0.28f;
            DrawTexturePro(background,
                           { 0.0f, skyCut, (float)background.width, background.height - skyCut },
                           { area.x, area.y, area.width, area.height - grassHeight },
                           { 0.0f, 0.0f }, 0.0f, WHITE);
            if (IsTextureValid(previewGrass[mapIndex]))
            {
                const Texture2D grass = previewGrass[mapIndex];
                DrawTexturePro(grass, { 0.0f, 0.0f, (float)grass.width, (float)grass.height },
                               { area.x, area.y + area.height - grassHeight, area.width, grassHeight },
                               { 0.0f, 0.0f }, 0.0f, WHITE);
            }
            return;
        }

        const Color skyTop = id == StadiumId::Classic ? Color{ 48, 107, 177, 255 }
            : id == StadiumId::NightCity ? Color{ 18, 27, 68, 255 }
            : Color{ 59, 190, 211, 255 };
        const Color skyBottom = id == StadiumId::Classic ? Color{ 156, 207, 238, 255 }
            : id == StadiumId::NightCity ? Color{ 63, 54, 117, 255 }
            : Color{ 255, 217, 134, 255 };
        const int x = static_cast<int>(area.x);
        const int y = static_cast<int>(area.y);
        const int width = static_cast<int>(area.width);
        const int height = static_cast<int>(area.height);

        DrawRectangleGradientV(x, y, width, height, skyTop, skyBottom);

        if (id == StadiumId::Classic)
        {
            DrawCircle(x + width - 58, y + 42, 22.0f, GOLD);
            DrawRectangle(x + 24, y + 28, 22, height / 2, Fade(WHITE, 0.45f));
            DrawRectangle(x + width - 46, y + 28, 22, height / 2, Fade(WHITE, 0.45f));
            DrawRectangle(x, y + height - 38, width, 38, { 61, 171, 82, 255 });
            DrawLine(x, y + height - 38, x + width, y + height - 38, WHITE);
            DrawCircleLines(x + width / 2, y + height - 19, 25.0f, Fade(WHITE, 0.8f));
            DrawRectangle(x + 10, y + height - 54, 9, 16, WHITE);
            DrawRectangle(x + width - 19, y + height - 54, 9, 16, WHITE);
        }
        else if (id == StadiumId::NightCity)
        {
            const int buildingWidths[] = { 38, 52, 33, 61, 42, 55, 36 };
            int buildingX = x + 8;
            for (int i = 0; i < 7; ++i)
            {
                const int buildingHeight = 28 + (i * 19 % 54);
                DrawRectangle(buildingX, y + height - 28 - buildingHeight,
                              buildingWidths[i], buildingHeight, { 21, 28, 64, 255 });
                DrawRectangle(buildingX + buildingWidths[i] / 2 - 2,
                              y + height - 24 - buildingHeight,
                              4, buildingHeight, Fade(Stadiums::at(mapIndex).accent, 0.75f));
                buildingX += buildingWidths[i] + 8;
            }
            DrawRectangle(x, y + height - 28, width, 28, { 45, 54, 90, 255 });
            DrawLine(x, y + height - 28, x + width, y + height - 28, Stadiums::at(mapIndex).accent);
        }
        else
        {
            DrawCircle(x + width - 56, y + 40, 23.0f, { 255, 229, 139, 255 });
            DrawRectangle(x, y + height - 56, width, 56, { 54, 184, 202, 255 });
            DrawRectangle(x, y + height - 30, width, 30, { 237, 195, 110, 255 });
            DrawTriangle({ static_cast<float>(x + 45), static_cast<float>(y + height - 48) },
                         { static_cast<float>(x + 76), static_cast<float>(y + height - 48) },
                         { static_cast<float>(x + 60), static_cast<float>(y + 85) },
                         { 32, 132, 79, 255 });
            DrawRectangle(x + 58, y + height - 72, 5, 28, { 117, 76, 43, 255 });
        }
    }

void MenuState::drawMapCard(int mapIndex, float position) const
{
        const StadiumInfo& stadium = Stadiums::at(mapIndex);
        const float distance = static_cast<float>(mapIndex) - position;
        if (std::abs(distance) > 1.15f)
        {
            return;
        }

        const Rectangle card = getCardBounds(mapIndex, position);
        const bool selected = mapIndex == static_cast<int>(std::round(position));
        const float scale = card.width / 420.0f;
        const float opacity = selected ? 1.0f : 0.76f;

        DrawRectangleRounded(card, 0.08f, 12, Fade({ 14, 23, 43, 255 }, opacity));

        const Rectangle preview = {
            card.x + 8.0f,
            card.y + 8.0f,
            card.width - 16.0f,
            card.height * 0.62f
        };
        drawMapPreview(preview, mapIndex);

        const int titleSize = static_cast<int>(22.0f * scale);
        DrawText(stadium.name,
                 static_cast<int>(card.x + 17.0f),
                 static_cast<int>(preview.y + preview.height + 12.0f * scale),
                 titleSize, Fade(WHITE, opacity));

        const char* status = stadium.available ? "DISPONIBLE" : "PROXIMAMENTE";
        const Color statusColor = stadium.available ? stadium.accent : LIGHTGRAY;
        DrawText(status,
                 static_cast<int>(card.x + 17.0f),
                 static_cast<int>(card.y + card.height - 30.0f * scale),
                 static_cast<int>(14.0f * scale), Fade(statusColor, opacity));

        if (selected)
        {
            DrawRectangleRoundedLinesEx(card, 0.08f, 12, 3.0f, stadium.accent);
        }
}

void MenuState::handleInput()
{
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
    {
        selectedMap = std::max(0, selectedMap - 1);
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
    {
        selectedMap = std::min(MAP_COUNT - 1, selectedMap + 1);
    }

    const Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(mouse, LEFT_BUTTON))
        {
            selectedMap = std::max(0, selectedMap - 1);
        }
        else if (CheckCollisionPointRec(mouse, RIGHT_BUTTON))
        {
            selectedMap = std::min(MAP_COUNT - 1, selectedMap + 1);
        }
        else
        {
            for (int mapIndex = 0; mapIndex < MAP_COUNT; ++mapIndex)
            {
                if (std::abs(static_cast<float>(mapIndex) - carouselPosition) <= 1.15f &&
                    CheckCollisionPointRec(mouse, getCardBounds(mapIndex, carouselPosition)))
                {
                    selectedMap = mapIndex;
                    break;
                }
            }

            if (CheckCollisionPointRec(mouse, START_BUTTON) && Stadiums::at(selectedMap).available)
            {
                this->state_machine->add_state(
                    std::make_unique<MainGameState>(Stadiums::at(selectedMap).id), true);
            }
        }
    }

    // La partida empieza en el estadio seleccionado en el carrusel
    if ((IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) && Stadiums::at(selectedMap).available)
    {
        this->state_machine->add_state(
            std::make_unique<MainGameState>(Stadiums::at(selectedMap).id), true);
    }
}

void MenuState::update(float deltaTime)
{
    const float easing = 1.0f - std::exp(-10.0f * deltaTime);
    carouselPosition += (static_cast<float>(selectedMap) - carouselPosition) * easing;
}

void MenuState::render()
{
    BeginDrawing();
    DrawRectangleGradientV(0, 0, 1080, 720, { 13, 23, 47, 255 }, { 28, 42, 72, 255 });

    for (int y = 30; y < 720; y += 86)
    {
        DrawLine(0, y, 1080, y, Fade(SKYBLUE, 0.07f));
    }

    const char* title = "HEAD SOCCER CLASH";
    DrawText(title, static_cast<int>(SCREEN_CENTER_X - MeasureText(title, 38) / 2.0f),
             35, 38, WHITE);
    const char* subtitle = "SELECCIONA TU CANCHA";
    DrawText(subtitle, static_cast<int>(SCREEN_CENTER_X - MeasureText(subtitle, 20) / 2.0f),
             112, 20, SKYBLUE);

    for (int mapIndex = 0; mapIndex < MAP_COUNT; ++mapIndex)
    {
        drawMapCard(mapIndex, carouselPosition);
    }

    DrawCircle(58, 352, 32.0f, Fade(WHITE, 0.13f));
    DrawTriangle({ 67.0f, 335.0f }, { 48.0f, 352.0f }, { 67.0f, 369.0f }, WHITE);
    DrawCircle(1022, 352, 32.0f, Fade(WHITE, 0.13f));
    DrawTriangle({ 1013.0f, 335.0f }, { 1013.0f, 369.0f }, { 1032.0f, 352.0f }, WHITE);

    for (int mapIndex = 0; mapIndex < MAP_COUNT; ++mapIndex)
    {
        const float dotX = SCREEN_CENTER_X + (mapIndex - (MAP_COUNT - 1) / 2.0f) * 24.0f;
        DrawCircle(static_cast<int>(dotX), 520, mapIndex == selectedMap ? 7.0f : 4.0f,
                   mapIndex == selectedMap ? Stadiums::at(mapIndex).accent : Fade(WHITE, 0.45f));
    }

    const StadiumInfo& selected = Stadiums::at(selectedMap);
    DrawText(selected.description,
             static_cast<int>(SCREEN_CENTER_X - MeasureText(selected.description, 18) / 2.0f),
             535, 18, Fade(WHITE, 0.85f));

    const Color buttonColor = selected.available ? selected.accent : GRAY;
    DrawRectangleRounded(START_BUTTON, 0.25f, 12, buttonColor);
    const char* buttonText = selected.available ? "JUGAR" : "PROXIMAMENTE";
    DrawText(buttonText,
             static_cast<int>(SCREEN_CENTER_X - MeasureText(buttonText, 22) / 2.0f),
             576, 22, selected.available ? DARKGREEN : LIGHTGRAY);

    const char* controls = "FLECHAS / A-D: ELEGIR     ENTER: JUGAR     RATON: NAVEGAR";
    DrawText(controls, static_cast<int>(SCREEN_CENTER_X - MeasureText(controls, 16) / 2.0f),
             660, 16, Fade(WHITE, 0.65f));
    EndDrawing();
}
