#include <MainGameState.hpp>
#include <GameOverState.hpp>
#include <StateMachine.hpp>
#include <iostream>

#include "debug/DebugDraw.hpp"
#include "physics/PhysicsGeometry.hpp"

// Constructor: Inicializamos el marcador a 0
MainGameState::MainGameState()
    : scoreP1(0), scoreP2(0), debugEnabled(false),
      trajectoryEnabled(false), lastDeltaTime(1.0f / 60.0f)
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
    this->matchTimer = 90.0f;

    // Configuración de dimensiones para las porterías
    float goalWidth = 80.0f;
    float goalHeight = 150.0f;
    float crossbarThickness = 12.0f;
    float groundY = PhysicsGeometry::groundY;

    // Portería Izquierda (J1)
    this->goalP1.netArea = { 0.0f, groundY - goalHeight, goalWidth, goalHeight };
    this->goalP1.crossbar = { 0.0f, groundY - goalHeight, goalWidth, crossbarThickness };

    // Portería Derecha (J2)
    this->goalP2.netArea = {
        PhysicsGeometry::fieldWidth - goalWidth,
        groundY - goalHeight,
        goalWidth,
        goalHeight
    };
    this->goalP2.crossbar = {
        PhysicsGeometry::fieldWidth - goalWidth,
        groundY - goalHeight,
        goalWidth,
        crossbarThickness
    };

    // Colocamos las entidades en posición de saque
    resetPositions();
}

void MainGameState::handleInput()
{
    if (IsKeyPressed(KEY_F1))
    {
        debugEnabled = !debugEnabled;
        if (!debugEnabled)
        {
            trajectoryEnabled = false;
        }
    }

    if (debugEnabled && IsKeyPressed(KEY_F2))
    {
        trajectoryEnabled = !trajectoryEnabled;
    }

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
    lastDeltaTime = deltaTime;

    this->matchTimer -= deltaTime;
    if (this->matchTimer <= 0.0f)
    {
        this->matchTimer = 0.0f;
        // Cuando se agota el tiempo, cambiamos a GameOverState pasando los marcadores
        this->state_machine->add_state(std::make_unique<GameOverState>(scoreP1, scoreP2), true);
        return;
    }

    // 1. Integración de movimiento
    this->player1.x += this->player1.vx;
    this->player1.y += this->player1.vy;

    this->player2.x += this->player2.vx;
    this->player2.y += this->player2.vy;

    this->ball.x += this->ball.vx;
    this->ball.y += this->ball.vy;

    // Cajas de colisión (AABB)
    Rectangle ballRect = PhysicsGeometry::ballBounds(ball.x, ball.y);
    Rectangle player1Rect = PhysicsGeometry::playerBounds(player1.x, player1.y);
    Rectangle player2Rect = PhysicsGeometry::playerBounds(player2.x, player2.y);

    float suelo = PhysicsGeometry::groundY;
    float altoJugador = player1Rect.height;
    float altoPelota = PhysicsGeometry::ballRadius;

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
    if (ball.x + altoPelota >= PhysicsGeometry::fieldWidth &&
        ball.y < goalP2.netArea.y) {
        ball.x = PhysicsGeometry::fieldWidth - altoPelota;
        ball.vx = -ball.vx * 0.7f;
    }

    ballRect = PhysicsGeometry::ballBounds(ball.x, ball.y);

    // 5. Rebote en Largueros
    if (CheckCollisionRecs(ballRect, goalP1.crossbar)) {
        ball.vy = -ball.vy * 0.8f;
        ball.y = goalP1.crossbar.y - altoPelota;
        ballRect = PhysicsGeometry::ballBounds(ball.x, ball.y);
    }
    if (CheckCollisionRecs(ballRect, goalP2.crossbar)) {
        ball.vy = -ball.vy * 0.8f;
        ball.y = goalP2.crossbar.y - altoPelota;
        ballRect = PhysicsGeometry::ballBounds(ball.x, ball.y);
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
    ballRect = PhysicsGeometry::ballBounds(ball.x, ball.y);
    player1Rect = PhysicsGeometry::playerBounds(player1.x, player1.y);
    player2Rect = PhysicsGeometry::playerBounds(player2.x, player2.y);

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
    DrawRectangle(
        0,
        static_cast<int>(PhysicsGeometry::groundY),
        static_cast<int>(PhysicsGeometry::fieldWidth),
        120,
        GREEN
    );
    DrawLine(
        0,
        static_cast<int>(PhysicsGeometry::groundY),
        static_cast<int>(PhysicsGeometry::fieldWidth),
        static_cast<int>(PhysicsGeometry::groundY),
        DARKGREEN
    );

    // Porterías (Redes y Largueros)
    DrawRectangleRec(goalP1.netArea, LIGHTGRAY);
    DrawRectangleRec(goalP2.netArea, LIGHTGRAY);
    DrawRectangleRec(goalP1.crossbar, DARKGRAY);
    DrawRectangleRec(goalP2.crossbar, DARKGRAY);

    // Jugadores y Pelota
    DrawRectangleRec(
        PhysicsGeometry::playerBounds(player1.x, player1.y), RED
    );

    DrawRectangleRec(
        PhysicsGeometry::playerBounds(player2.x, player2.y), BLUE
    );

    DrawCircleV(
        {ball.x, ball.y}, PhysicsGeometry::ballRadius, BLACK
    );

    // Marcador (HUD)
    std::string scoreText = std::to_string(scoreP1) + "  -  " + std::to_string(scoreP2);
    int fontSize = 40;
    int textWidth = MeasureText(scoreText.c_str(), fontSize);
    DrawText(scoreText.c_str(), 540 - (textWidth / 2), 25, fontSize, BLACK);

    int minutes = (int)matchTimer / 60;
    int seconds = (int)matchTimer % 60;
    char timerBuffer[16];
    snprintf(timerBuffer, sizeof(timerBuffer), "%02d:%02d", minutes, seconds);

    Color timerColor = (matchTimer <= 10.0f) ? RED : DARKGRAY; // Se pone rojo en los últimos 10 segundos
    DrawText(timerBuffer, 540 - (MeasureText(timerBuffer, 30) / 2), 65, 30, timerColor);

    if (debugEnabled)
    {
        renderDebug();
    }

    EndDrawing();
}

void MainGameState::renderDebug()
{
    if (trajectoryEnabled)
    {
        renderBallTrajectory();
    }

    Rectangle player1Bounds = PhysicsGeometry::playerBounds(player1.x, player1.y);
    Rectangle player2Bounds = PhysicsGeometry::playerBounds(player2.x, player2.y);
    Rectangle ballBounds = PhysicsGeometry::ballBounds(ball.x, ball.y);
    Vector2 ballCenter = {ball.x, ball.y};

    DebugDraw::box(player1Bounds, LIME);
    DebugDraw::box(player2Bounds, LIME);
    DebugDraw::box(ballBounds, GOLD);
    DrawCircleLinesV(ballCenter, PhysicsGeometry::ballRadius, GRAY);

    DebugDraw::box(goalP1.netArea, PURPLE);
    DebugDraw::box(goalP2.netArea, PURPLE);
    DebugDraw::box(goalP1.crossbar, ORANGE);
    DebugDraw::box(goalP2.crossbar, ORANGE);

    DrawLineEx(
        {0.0f, PhysicsGeometry::groundY},
        {PhysicsGeometry::fieldWidth, PhysicsGeometry::groundY},
        2.0f,
        LIME
    );

    DrawLineEx(
        {0.0f, 0.0f},
        {0.0f, goalP1.netArea.y},
        2.0f,
        ORANGE
    );

    DrawLineEx(
        {PhysicsGeometry::fieldWidth, 0.0f},
        {PhysicsGeometry::fieldWidth, goalP2.netArea.y},
        2.0f,
        ORANGE
    );

    const float velocityScale = 8.0f;
    DebugDraw::arrow(
        PhysicsGeometry::center(player1Bounds),
        {player1.vx, player1.vy},
        velocityScale,
        SKYBLUE
    );
    DebugDraw::arrow(
        PhysicsGeometry::center(player2Bounds),
        {player2.vx, player2.vy},
        velocityScale,
        SKYBLUE
    );
    DebugDraw::arrow(
        ballCenter,
        {ball.vx, ball.vy},
        velocityScale,
        SKYBLUE
    );

    DrawCircleV(ballCenter, 3.0f, MAGENTA);
    const char* trajectoryMessage = trajectoryEnabled
        ? "F2: ocultar trayectoria"
        : "F2: mostrar trayectoria";
    DrawText("Modo depuracion: F1 para cerrar", 20, 100, 18, DARKGRAY);
    DrawText(trajectoryMessage, 20, 122, 18, DARKGRAY);
    if (trajectoryEnabled)
    {
        DrawText("Prediccion aproximada; jugadores inmoviles", 20, 144, 14, GRAY);
    }
}

void MainGameState::renderBallTrajectory()
{
    Ball predictedBall = ball;
    Rectangle player1Bounds = PhysicsGeometry::playerBounds(player1.x, player1.y);
    Rectangle player2Bounds = PhysicsGeometry::playerBounds(player2.x, player2.y);

    float predictionDeltaTime = lastDeltaTime;
    if (predictionDeltaTime <= 0.0f)
    {
        predictionDeltaTime = 1.0f / 60.0f;
    }

    Vector2 previousPosition = {predictedBall.x, predictedBall.y};
    DrawCircleV(previousPosition, 3.0f, SKYBLUE);

    const int maximumSteps = 240;
    for (int step = 0; step < maximumSteps; step++)
    {
        predictedBall.x += predictedBall.vx;
        predictedBall.y += predictedBall.vy;

        bool bounced = false;
        const float ballRadius = PhysicsGeometry::ballRadius;

        if (predictedBall.y + ballRadius >= PhysicsGeometry::groundY)
        {
            predictedBall.y = PhysicsGeometry::groundY - ballRadius;
            predictedBall.vy = -predictedBall.vy * 0.6f;
            bounced = true;
        }

        if (predictedBall.x - ballRadius <= 0.0f &&
            predictedBall.y < goalP1.netArea.y)
        {
            predictedBall.x = ballRadius;
            predictedBall.vx = -predictedBall.vx * 0.7f;
            bounced = true;
        }

        if (predictedBall.x + ballRadius >= PhysicsGeometry::fieldWidth &&
            predictedBall.y < goalP2.netArea.y)
        {
            predictedBall.x = PhysicsGeometry::fieldWidth - ballRadius;
            predictedBall.vx = -predictedBall.vx * 0.7f;
            bounced = true;
        }

        Rectangle predictedBounds = PhysicsGeometry::ballBounds(
            predictedBall.x,
            predictedBall.y
        );

        if (CheckCollisionRecs(predictedBounds, goalP1.crossbar))
        {
            predictedBall.vy = -predictedBall.vy * 0.8f;
            predictedBall.y = goalP1.crossbar.y - ballRadius;
            predictedBounds = PhysicsGeometry::ballBounds(
                predictedBall.x,
                predictedBall.y
            );
            bounced = true;
        }

        if (CheckCollisionRecs(predictedBounds, goalP2.crossbar))
        {
            predictedBall.vy = -predictedBall.vy * 0.8f;
            predictedBall.y = goalP2.crossbar.y - ballRadius;
            predictedBounds = PhysicsGeometry::ballBounds(
                predictedBall.x,
                predictedBall.y
            );
            bounced = true;
        }

        Vector2 currentPosition = {predictedBall.x, predictedBall.y};
        DrawLineEx(previousPosition, currentPosition, 2.0f, Fade(SKYBLUE, 0.8f));

        if (step % 8 == 0)
        {
            DrawCircleV(currentPosition, 2.5f, SKYBLUE);
        }

        if (bounced)
        {
            DrawCircleV(currentPosition, 4.0f, ORANGE);
        }

        if (CheckCollisionPointRec(currentPosition, goalP1.netArea) ||
            CheckCollisionPointRec(currentPosition, goalP2.netArea))
        {
            DrawCircleV(currentPosition, 5.0f, GREEN);
            return;
        }

        if (CheckCollisionRecs(predictedBounds, player1Bounds) ||
            CheckCollisionRecs(predictedBounds, player2Bounds))
        {
            DrawCircleV(currentPosition, 5.0f, RED);
            return;
        }

        physics.Gravity(predictedBall.vy, predictionDeltaTime);
        physics.ApplyFriction(predictedBall.vx, 2.0f, predictionDeltaTime);
        previousPosition = currentPosition;
    }

    DrawCircleV(previousPosition, 4.0f, GRAY);
}
