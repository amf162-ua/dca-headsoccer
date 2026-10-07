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
    DrawText("Jugador 1: Teclas A, D para mover | W para saltar", 540 - MeasureText("Jugador 1: Teclas A, D para mover | W para saltar", 20) / 2, 500, 20, GRAY);
    DrawText("Jugador 2: Flechas Izq, Der para mover | Arriba para saltar", 540 - MeasureText("Jugador 2: Flechas Izq, Der para mover | Arriba para saltar", 20) / 2, 535, 20, GRAY);

    EndDrawing();
}