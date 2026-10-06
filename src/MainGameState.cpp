#include <MainGameState.hpp>
#include <iostream>

// Constructor: Inicializamos el marcador a 0
MainGameState::MainGameState() : scoreP1(0), scoreP2(0)
{
}

// Método privado para colocar jugadores y pelota en posición de saque
void MainGameState::resetPositions()
{
    // Jugador 1 (Izquierda)
    this->player1.x = 100.f;
    this->player1.y = 520.f;
    this->player1.vx = 0.f;
    this->player1.vy = 0.f;

    // Jugador 2 (Derecha)
    this->player2.x = 830.f;
    this->player2.y = 520.f;
    this->player2.vx = 0.f;
    this->player2.vy = 0.f;

    // Pelota en el centro de la pantalla (1080 / 2 = 540)
    this->ball.x = 540.f;
    this->ball.y = 400.f;
    this->ball.vx = 0.f;
    this->ball.vy = 0.f;
}

void MainGameState::init()
{
    // Reseteamos el marcador
    this->scoreP1 = 0;
    this->scoreP2 = 0;

    // Configuración de dimensiones para las porterías
    float goalWidth = 80.0f;
    float goalHeight = 150.0f;
    float crossbarThickness = 12.0f;
    float groundY = 600.0f;

    // Portería Izquierda (J1)
    this->goalP1.netArea = { 0.0f, groundY - goalHeight, goalWidth, goalHeight };
    this->goalP1.crossbar = { 0.0f, groundY - goalHeight, goalWidth, crossbarThickness };

    // Portería Derecha (J2)
    this->goalP2.netArea = { 1080.0f - goalWidth, groundY - goalHeight, goalWidth, goalHeight };
    this->goalP2.crossbar = { 1080.0f - goalWidth, groundY - goalHeight, goalWidth, crossbarThickness };

    // Colocamos las entidades en posición de saque
    resetPositions();
}

void MainGameState::handleInput()
{
    // Player 1
    if (IsKeyDown(KEY_A))
    {
        player1.vx = -5.0f;
    } 
    else if (IsKeyDown(KEY_D))
    {
        player1.vx = 5.0f;
    } 
    else
    {
        player1.vx = 0.0f;
    }

    if (IsKeyPressed(KEY_W))
    {
        player1.vy = -7.0f;
    } 
    else 
    {
        this->physics.Gravity(player1.vy, 0.016f);
    }

    if (IsKeyDown(KEY_SPACE))
    {
        // Chutar
    }

    // Player 2
    if (IsKeyDown(KEY_LEFT))
    {
        player2.vx = -5.0f;
    } 
    else if (IsKeyDown(KEY_RIGHT))
    {
        player2.vx = 5.0f;
    } 
    else
    {
        player2.vx = 0.0f;
    }

    if (IsKeyPressed(KEY_UP))
    {
        player2.vy = -7.0f;
    } 
    else 
    {
        this->physics.Gravity(player2.vy, 0.016f);
    }

    if (IsKeyDown(KEY_KP_ENTER))
    {
        // Chutar
    }
}

void MainGameState::update(float deltaTime)
{
    // 1. Integración de movimiento
    this->player1.x += this->player1.vx;
    this->player1.y += this->player1.vy;

    this->player2.x += this->player2.vx;
    this->player2.y += this->player2.vy;

    this->ball.x += this->ball.vx;
    this->ball.y += this->ball.vy;

    // Cajas de colisión (AABB)
    Rectangle ballRect = {ball.x - 20, ball.y - 20, 40.0f, 40.0f};
    Rectangle player1Rect = {player1.x, player1.y, 50.0f, 80.0f};
    Rectangle player2Rect = {player2.x, player2.y, 50.0f, 80.0f};

    float suelo = 600.0f;
    float altoJugador = player1Rect.height;
    float altoPelota = ballRect.height / 2;

    // 2. Colisión de Jugadores con el Suelo
    if (player1.y + altoJugador >= suelo) {
        player1.y = suelo - altoJugador;
        player1.vy = 0.0f;
    }

    if (player2.y + altoJugador >= suelo) {
        player2.y = suelo - altoJugador;
        player2.vy = 0.0f;
    }

    // 3. Colisión de Pelota con el Suelo
    if (ball.y + altoPelota >= suelo) {
        ball.y = suelo - altoPelota;
        ball.vy = -ball.vy * 0.6f; // Rebote elástico
    }

    // 4. Colisión con Paredes Laterales (Solo por encima del larguero)
    if (ball.x - altoPelota <= 0 && ball.y < goalP1.netArea.y) {
        ball.x = altoPelota;
        ball.vx = -ball.vx * 0.7f;
    }
    if (ball.x + altoPelota >= 1080 && ball.y < goalP2.netArea.y) {
        ball.x = 1080 - altoPelota;
        ball.vx = -ball.vx * 0.7f;
    }

    // 5. Rebote en Largueros
    if (CheckCollisionRecs(ballRect, goalP1.crossbar)) {
        ball.vy = -ball.vy * 0.8f;
        ball.y = goalP1.crossbar.y - altoPelota;
    }
    if (CheckCollisionRecs(ballRect, goalP2.crossbar)) {
        ball.vy = -ball.vy * 0.8f;
        ball.y = goalP2.crossbar.y - altoPelota;
    }

    // 6. Detección de Gol
    Vector2 ballCenter = { ball.x, ball.y };

    if (CheckCollisionPointRec(ballCenter, goalP1.netArea)) {
        this->scoreP2++;
        std::cout << "¡GOL DEL JUGADOR 2! Marcador: " << scoreP1 << " - " << scoreP2 << std::endl;
        resetPositions();
        return;
    }

    if (CheckCollisionPointRec(ballCenter, goalP2.netArea)) {
        this->scoreP1++;
        std::cout << "¡GOL DEL JUGADOR 1! Marcador: " << scoreP1 << " - " << scoreP2 << std::endl;
        resetPositions();
        return;
    }

    // 7. Colisiones Pelota - Jugadores
    if (CheckCollisionRecs(ballRect, player1Rect)) {
        if (ballRect.x + ballRect.height / 2 >= player1.x + player1Rect.width) ball.vx = player1.vx + 0.5f;
        if (ballRect.x + ballRect.height / 2 <= player1.x) ball.vx = player1.vx - 0.5f;
        if (ballRect.y - ballRect.height / 2 <= player1.y) ball.vy = -5.0f;
        if (ballRect.y + ballRect.height / 2 >= player1.y + player1Rect.height) ball.vy = 5.0f;
    }

    if (CheckCollisionRecs(ballRect, player2Rect)) {
        if (ballRect.x + ballRect.height / 2 >= player2.x + player2Rect.width) ball.vx = player2.vx + 0.5f;
        if (ballRect.x + ballRect.height / 2 <= player2.x) ball.vx = player2.vx - 0.5f;
        if (ballRect.y - ballRect.height / 2 <= player2.y) ball.vy = -5.0f;
        if (ballRect.y + ballRect.height / 2 >= player2.y + player2Rect.height) ball.vy = 5.0f;
    }

    // 8. Aplicación de física
    this->physics.Gravity(this->ball.vy, deltaTime);
    this->physics.Gravity(this->player1.vy, deltaTime);
    this->physics.Gravity(this->player2.vy, deltaTime);
    this->physics.ApplyFriction(this->ball.vx, 2.0f, deltaTime);
}

void MainGameState::render()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Suelo
    DrawRectangle(0, 600, 1080, 120, GREEN);
    DrawLine(0, 600, 1080, 600, DARKGREEN);

    // Porterías (Redes y Largueros)
    DrawRectangleRec(goalP1.netArea, LIGHTGRAY);
    DrawRectangleRec(goalP2.netArea, LIGHTGRAY);
    DrawRectangleRec(goalP1.crossbar, DARKGRAY);
    DrawRectangleRec(goalP2.crossbar, DARKGRAY);

    // Jugadores y Pelota
    DrawRectangle((int)player1.x, (int)player1.y, 50, 80, RED);
    DrawRectangle((int)player2.x, (int)player2.y, 50, 80, BLUE);
    DrawCircle((int)ball.x, (int)ball.y, 20, BLACK);

    // Marcador (HUD)
    std::string scoreText = std::to_string(scoreP1) + "  -  " + std::to_string(scoreP2);
    int fontSize = 40;
    int textWidth = MeasureText(scoreText.c_str(), fontSize);
    DrawText(scoreText.c_str(), 540 - (textWidth / 2), 25, fontSize, BLACK);

    EndDrawing();
}