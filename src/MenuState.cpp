#include <MenuState.hpp>
#include <MainGameState.hpp>
#include <StateMachine.hpp>

MenuState::MenuState()
{
}

void MenuState::init()
{
}

void MenuState::handleInput()
{
    // Al pulsar ENTER, reemplazamos el MenuState por MainGameState (comienza el partido)
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
    {
        this->state_machine->add_state(std::make_unique<MainGameState>(), true);
    }
}

void MenuState::update(float deltaTime)
{
}

void MenuState::render()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Título Principal
    const char* title = "HEAD SOCCER CLASH";
    int titleFontSize = 50;
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(title, 540 - (titleWidth / 2), 220, titleFontSize, DARKBLUE);

    // Subtítulo / Instrucciones
    const char* startPrompt = "Pulsa ENTER para Jugar";
    int promptFontSize = 25;
    int promptWidth = MeasureText(startPrompt, promptFontSize);
    DrawText(startPrompt, 540 - (promptWidth / 2), 380, promptFontSize, DARKGRAY);

    // Controles Básicos
    const char* controlsP1 = "Jugador 1 (Rojo): A, D mover | W saltar | ESPACIO chutar";
    int p1Width = MeasureText(controlsP1, 20);
    DrawText(controlsP1, 540 - (p1Width / 2), 490, 20, MAROON);

    const char* controlsP2 = "Jugador 2 (Azul): Flechas mover | Arriba saltar | L chutar";
    int p2Width = MeasureText(controlsP2, 20);
    DrawText(controlsP2, 540 - (p2Width / 2), 530, 20, DARKBLUE);

    EndDrawing();
}