#include <MainGameState.hpp>
#include <iostream>

MainGameState::MainGameState()
{
}

void MainGameState::init()
{
    this->player1.x = 100.f;
    this->player1.y = 520.f;
    this->player1.vx = 0.f;
    this->player1.vy = 0.f;

    this->player2.x = 830.f;
    this->player2.y = 520.f;
    this->player2.vx = 0.f;
    this->player2.vy = 0.f;

    this->ball.x = 500.f;
    this->ball.y = 550.f;
    this->ball.vx = 0.f;
    this->ball.vy = 0.f;
}

void MainGameState::handleInput()
{
    // Player 1
    if (IsKeyDown(KEY_A))
    {
        player1.vx = -5.0f;

    } else if (IsKeyDown(KEY_D))
    {
        player1.vx = 5.0f;
    } else
    {
        player1.vx = 0.0f;

    }

    if (IsKeyPressed(KEY_W))
    {
        player1.vy = -7.0f;
    } else {
        this->physics.Gravity(player1.vy, 0.016f);
    }

    if (IsKeyDown(KEY_SPACE))
    {
        //chutar
    }

    // Player 2
    if (IsKeyDown(KEY_LEFT))
    {
        player2.vx = -5.0f;
    } else if (IsKeyDown(KEY_RIGHT))
    {
        player2.vx = 5.0f;
    } else
    {
        player2.vx = 0.0f;

    }

    if (IsKeyPressed(KEY_UP))
    {
        player2.vy = -7.0f;
    } else {
        this->physics.Gravity(player2.vy, 0.016f);
    }

    if (IsKeyDown(KEY_KP_ENTER))
    {
        //chutar
    }
}

void MainGameState::update(float deltaTime)
{
    this->player1.x += this->player1.vx;
    this->player1.y += this->player1.vy;

    this->player2.x += this->player2.vx;
    this->player2.y += this->player2.vy;

    this->ball.x += this->ball.vx;
    this->ball.y += this->ball.vy;

    Rectangle player1Rect = {player1.x, player1.y, 50.0f, 80.0f};
    Rectangle player2Rect = {player2.x, player2.y, 50.0f, 80.0f};

    Rectangle ballRect = {ball.x, ball.y, 20.0f, 20.0f};

    float suelo = 600.0f;
    float altoJugador = player1Rect.height;
    float altoPelota = ballRect.height;

    if(player1.y + altoJugador >= suelo)
    {
        player1.y = suelo - altoJugador;
        player1.vy = 0.0f;
    }

    if(player2.y + altoJugador >= suelo)
    {
        player2.y = suelo - altoJugador;
        player2.vy = 0.0f;
    }

    if(ball.y + altoPelota >= suelo){
        
        ball.y = suelo - altoPelota;
        ball.vy = -ball.vy * 0.6f;
    }

    if(CheckCollisionRecs(ballRect, player1Rect))
    {
        ball.vx = 5.0f;

    } else if(CheckCollisionRecs(ballRect, player2Rect))
    {
        ball.vx = -5.0f;
    }

    this->physics.Gravity(this->ball.vy, deltaTime);
    this->physics.Gravity(this->player1.vy, deltaTime);
    this->physics.Gravity(this->player2.vy, deltaTime);
    this->physics.ApplyFriction(this->ball.vx, 2.0f, deltaTime);
}

void MainGameState::render()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(0,600,1080,120,GREEN);
    DrawLine(0,600,1080,600,DARKGREEN);

    DrawRectangle((int)player1.x, (int)player1.y, 50, 80, RED);
    DrawRectangle((int)player2.x, (int)player2.y, 50, 80, BLUE);
    DrawCircle((int)ball.x, (int)ball.y, 20,BLACK);
    EndDrawing();

}