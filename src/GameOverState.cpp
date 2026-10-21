#include <GameOverState.hpp>
#include <MainGameState.hpp>
#include <MenuState.hpp>
#include <StateMachine.hpp>

// Evaluamos el resultado del partido directamente en el constructor
GameOverState::GameOverState(int score1, int score2) 
    : finalScoreP1(score1), finalScoreP2(score2)
{
    if (finalScoreP1 > finalScoreP2) {
        winnerMessage = "¡GANADOR: JUGADOR 1 (ROJO)!";
    } else if (finalScoreP2 > finalScoreP1) {
        winnerMessage = "¡GANADOR: JUGADOR 2 (AZUL)!";
    } else {
        winnerMessage = "¡EMPATE TÉCNICO!";
    }
}

void GameOverState::init()
{
}

void GameOverState::handleInput()
{
    // Pulsa R -> Volver a Jugar (Reemplaza GameOverState por un nuevo MainGameState)
    if (IsKeyPressed(KEY_R))
    {
        this->state_machine->add_state(std::make_unique<MainGameState>(), true);
    }

    // Pulsa M -> Volver al Menú Principal (Reemplaza GameOverState por MenuState)
    if (IsKeyPressed(KEY_M))
    {
        this->state_machine->add_state(std::make_unique<MenuState>(), true);
    }
}

void GameOverState::update(float deltaTime)
{
}

void GameOverState::render()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Título de Fin de Partido
    const char* title = "FIN DEL PARTIDO";
    int titleFontSize = 45;
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(title, 540 - (titleWidth / 2), 150, titleFontSize, BLACK);

    // Mensaje de Ganador
    Color resultColor = (finalScoreP1 > finalScoreP2) ? RED : ((finalScoreP2 > finalScoreP1) ? BLUE : DARKGRAY);
    int winnerFontSize = 35;
    int winnerWidth = MeasureText(winnerMessage.c_str(), winnerFontSize);
    DrawText(winnerMessage.c_str(), 540 - (winnerWidth / 2), 240, winnerFontSize, resultColor);

    // Marcador Final
    std::string scoreStr = "Marcador Final: " + std::to_string(finalScoreP1) + "  -  " + std::to_string(finalScoreP2);
    int scoreFontSize = 30;
    int scoreWidth = MeasureText(scoreStr.c_str(), scoreFontSize);
    DrawText(scoreStr.c_str(), 540 - (scoreWidth / 2), 320, scoreFontSize, DARKGRAY);

    // Opciones del Menú
    DrawText("Pulsa [ R ] para Revancha", 540 - MeasureText("Pulsa [ R ] para Revancha", 22) / 2, 450, 22, DARKGREEN);
    DrawText("Pulsa [ M ] para ir al Menú Principal", 540 - MeasureText("Pulsa [ M ] para ir al Menú Principal", 22) / 2, 490, 22, DARKGRAY);

    EndDrawing();
}