#include <MainGameState.hpp>
#include <GameOverState.hpp>
#include <StateMachine.hpp>
#include <iostream>
#include <cmath>

/*
    Todas las velocidades van en px/s y las aceleraciones en px/s^2 (se multiplican por deltaTime).
    Los valores son los de antes (que iban por frame) multiplicados por 60, para que el juego se sienta igual.
*/
namespace
{
    const float GROUND_Y = 600.0f;
    const float FIELD_WIDTH = 1080.0f;

    const float PLAYER_WIDTH = 50.0f;
    const float PLAYER_HEIGHT = 80.0f;
    const float PLAYER_SPEED = 300.0f;  // Antes 5 px/frame
    const float JUMP_SPEED = -420.0f;   // Antes -7 px/frame

    const float BALL_RADIUS = 20.0f;
    const float BALL_FRICTION = 120.0f;   // Antes 2 px/frame por segundo
    const float CROSSBAR_PUSH = 120.0f;   // Empujon si la pelota se queda parada en el larguero (antes 2 px/frame)

    // Restitucion (e) de cada choque: 1 = rebote perfecto, 0 = no rebota
    const float E_GROUND = 0.6f;
    const float E_WALL = 0.7f;
    const float E_CROSSBAR = 0.8f;
    const float E_PLAYER = 0.8f;
}

// Constructor: Inicializamos el marcador a 0
MainGameState::MainGameState() : scoreP1(0), scoreP2(0)
{
}

// Método privado para colocar jugadores y pelota en posición de saque
void MainGameState::resetPositions()
{
    // Las posiciones de Body son el CENTRO del cuerpo (esquina + mitad del tamaño)

    // Jugador 1 (Izquierda): esquina en (100, 520)
    this->player1.body.setPosition(100.f + PLAYER_WIDTH / 2, 520.f + PLAYER_HEIGHT / 2);
    this->player1.body.setVelocity(0.f, 0.f);
    this->player1.isGrounded = true;

    // Jugador 2 (Derecha): esquina en (830, 520)
    this->player2.body.setPosition(830.f + PLAYER_WIDTH / 2, 520.f + PLAYER_HEIGHT / 2);
    this->player2.body.setVelocity(0.f, 0.f);
    this->player2.isGrounded = true;

    // Pelota en el centro de la pantalla (1080 / 2 = 540)
    this->ball.setPosition(540.f, 400.f);
    this->ball.setVelocity(0.f, 0.f);
}

void MainGameState::init()
{
    // Reseteamos el marcador
    this->scoreP1 = 0;
    this->scoreP2 = 0;
    this->matchTimer = 90.0f;

    // Cuerpos fisicos
    // Pelota: DYNAMIC, le afectan los choques
    this->ball.setCircleBody(BALL_RADIUS);
    this->ball.setBodyType(Body::bodyType::DYNAMIC);
    this->ball.setMass(1.0f);

    // Jugadores: ANIMATED, se mueven con el teclado pero la pelota no los empuja (invMass = 0)
    this->player1.body.setRectangleBody(PLAYER_WIDTH, PLAYER_HEIGHT);
    this->player1.body.setBodyType(Body::bodyType::ANIMATED);
    this->player2.body.setRectangleBody(PLAYER_WIDTH, PLAYER_HEIGHT);
    this->player2.body.setBodyType(Body::bodyType::ANIMATED);

    // Configuración de dimensiones para las porterías
    float goalWidth = 80.0f;
    float goalHeight = 150.0f;
    float crossbarThickness = 12.0f;
    float goalTop = GROUND_Y - goalHeight;

    // Portería Izquierda (J1)
    this->goalP1.netArea = { 0.0f, goalTop, goalWidth, goalHeight };
    this->goalP1.crossbar.setRectangleBody(goalWidth, crossbarThickness);
    this->goalP1.crossbar.setBodyType(Body::bodyType::STATIC);
    this->goalP1.crossbar.setPosition(goalWidth / 2, goalTop + crossbarThickness / 2);

    // Portería Derecha (J2)
    this->goalP2.netArea = { FIELD_WIDTH - goalWidth, goalTop, goalWidth, goalHeight };
    this->goalP2.crossbar.setRectangleBody(goalWidth, crossbarThickness);
    this->goalP2.crossbar.setBodyType(Body::bodyType::STATIC);
    this->goalP2.crossbar.setPosition(FIELD_WIDTH - goalWidth / 2, goalTop + crossbarThickness / 2);

    // Colocamos las entidades en posición de saque
    resetPositions();

    // Inicializamos el temporizador y cartel de gol
    this->goalBannerTimer = 0.0f;
    this->goalBannerText = "";
    this->goalBannerColor = BLACK;
}

void MainGameState::handleInput()
{
    Vector2& vel1 = player1.body.getVelocity();
    Vector2& vel2 = player2.body.getVelocity();

    // Si se está celebrando un gol, bloqueamos las entradas de los jugadores
    if (this->goalBannerTimer > 0.0f)
    {
        vel1.x = 0.0f;
        vel2.x = 0.0f;
        return;
    }

    // Player 1
    if (IsKeyDown(KEY_A))
    {
        vel1.x = -PLAYER_SPEED;
    }
    else if (IsKeyDown(KEY_D))
    {
        vel1.x = PLAYER_SPEED;
    }
    else
    {
        vel1.x = 0.0f;
    }

    // Salto: Solo si presiona W Y está en el suelo
    if (IsKeyPressed(KEY_W) && player1.isGrounded)
    {
        vel1.y = JUMP_SPEED;
        player1.isGrounded = false; // Bloqueamos saltar en el aire
    }

    if (IsKeyDown(KEY_SPACE))
    {
        // Chutar
    }

    // Player 2
    if (IsKeyDown(KEY_LEFT))
    {
        vel2.x = -PLAYER_SPEED;
    }
    else if (IsKeyDown(KEY_RIGHT))
    {
        vel2.x = PLAYER_SPEED;
    }
    else
    {
        vel2.x = 0.0f;
    }

    // Salto: Solo si presiona UP Y está en el suelo
    if (IsKeyPressed(KEY_UP) && player2.isGrounded)
    {
        vel2.y = JUMP_SPEED;
        player2.isGrounded = false; // Bloqueamos saltar en el aire
    }

    if (IsKeyDown(KEY_KP_ENTER))
    {
        // Chutar
    }
}

// Colisión del jugador con el suelo y con los bordes de la pantalla
void MainGameState::keepPlayerInField(Player& player)
{
    Vector2& pos = player.body.getPosition();
    Vector2& vel = player.body.getVelocity();
    float halfWidth = player.body.getWidth() / 2;
    float halfHeight = player.body.getHeight() / 2;

    // Suelo
    if (pos.y + halfHeight >= GROUND_Y) {
        pos.y = GROUND_Y - halfHeight;
        vel.y = 0.0f;
        player.isGrounded = true;
    }

    // Límites de pantalla: evita que el jugador se salga por los extremos
    if (pos.x - halfWidth < 0.0f) pos.x = halfWidth;
    if (pos.x + halfWidth > FIELD_WIDTH) pos.x = FIELD_WIDTH - halfWidth;
}

// Choque de la pelota contra un rectángulo (jugador o larguero) usando nuestras fisicas
Collision::Contact MainGameState::collideBallWith(Body& other, float restitution)
{
    Collision::Contact contact = Collision::detectCircleAABB(this->ball, other);
    Collision::resolveContact(this->ball, other, contact, restitution);
    return contact;
}

void MainGameState::update(float deltaTime)
{
    // Si hay un gol reciente, gestionamos la pausa para la celebración
    if (this->goalBannerTimer > 0.0f)
    {
        this->goalBannerTimer -= deltaTime;
        if (this->goalBannerTimer <= 0.0f)
        {
            this->goalBannerTimer = 0.0f;
            resetPositions();
        }
        return; // Durante el cartel de gol se pausa la simulación
    }

    this->matchTimer -= deltaTime;
    if (this->matchTimer <= 0.0f)
    {
        this->matchTimer = 0.0f;
        // Cuando se agota el tiempo, cambiamos a GameOverState pasando los marcadores
        this->state_machine->add_state(std::make_unique<GameOverState>(scoreP1, scoreP2), true);
        return;
    }

    Vector2& ballPos = this->ball.getPosition();
    Vector2& ballVel = this->ball.getVelocity();
    float ballRadius = this->ball.getRadius();

    // 1. Fuerzas: gravedad y fricción (cambian la velocidad)
    this->physics.Gravity(ballVel.y, deltaTime);
    this->physics.Gravity(this->player1.body.getVelocity().y, deltaTime);
    this->physics.Gravity(this->player2.body.getVelocity().y, deltaTime);
    this->physics.ApplyFriction(ballVel.x, BALL_FRICTION, deltaTime);

    // 2. Integración de movimiento (la velocidad cambia la posición)
    this->physics.Integrate(this->player1.body, deltaTime);
    this->physics.Integrate(this->player2.body, deltaTime);
    this->physics.Integrate(this->ball, deltaTime);

    // 3. Colisión de Jugadores con el Suelo
    keepPlayerInField(this->player1);
    keepPlayerInField(this->player2);

    // === COLISIÓN ENTRE JUGADORES (Ajustada al área real de los rectángulos) ===
    // Solo colisionan si sus cajas (X e Y) se tocan realmente en el espacio
    Rectangle player1Rect = player1.body.getRect();
    Rectangle player2Rect = player2.body.getRect();
    if (CheckCollisionRecs(player1Rect, player2Rect)) {
        Vector2& pos1 = player1.body.getPosition();
        Vector2& pos2 = player2.body.getPosition();
        Vector2& vel1 = player1.body.getVelocity();
        Vector2& vel2 = player2.body.getVelocity();

        float overlapX = (player1Rect.x < player2Rect.x)
            ? (player1Rect.x + player1Rect.width - player2Rect.x)
            : (player2Rect.x + player2Rect.width - player1Rect.x);

        float overlapY = (player1Rect.y < player2Rect.y)
            ? (player1Rect.y + player1Rect.height - player2Rect.y)
            : (player2Rect.y + player2Rect.height - player1Rect.y);

        // Si el choque es lateral (horizontal)
        if (overlapX < overlapY) {
            if (pos1.x < pos2.x) {
                pos1.x -= overlapX / 2.0f;
                pos2.x += overlapX / 2.0f;
                if (vel1.x > 0.0f) vel1.x = 0.0f;
                if (vel2.x < 0.0f) vel2.x = 0.0f;
            } else {
                pos1.x += overlapX / 2.0f;
                pos2.x -= overlapX / 2.0f;
                if (vel1.x < 0.0f) vel1.x = 0.0f;
                if (vel2.x > 0.0f) vel2.x = 0.0f;
            }
        } else {
            // Si el choque es vertical (uno aterriza sobre el otro)
            if (pos1.y < pos2.y) {
                pos1.y -= overlapY;
                vel1.y = 0.0f;
                player1.isGrounded = true;
            } else {
                pos2.y -= overlapY;
                vel2.y = 0.0f;
                player2.isGrounded = true;
            }
        }

        // El empujón puede haber sacado a alguno de la pantalla
        keepPlayerInField(this->player1);
        keepPlayerInField(this->player2);
    }

    // 4. Colisión de Pelota con el Suelo
    if (ballPos.y + ballRadius >= GROUND_Y) {
        ballPos.y = GROUND_Y - ballRadius;
        ballVel.y = -ballVel.y * E_GROUND; // Rebote elástico
    }

    // 5. Colisión con Paredes Laterales (Solo por encima del larguero)
    if (ballPos.x - ballRadius <= 0 && ballPos.y < goalP1.netArea.y) {
        ballPos.x = ballRadius;
        ballVel.x = -ballVel.x * E_WALL;
    }
    if (ballPos.x + ballRadius >= FIELD_WIDTH && ballPos.y < goalP2.netArea.y) {
        ballPos.x = FIELD_WIDTH - ballRadius;
        ballVel.x = -ballVel.x * E_WALL;
    }

    // 6. Rebote en Largueros (con nuestras fisicas: rebota bien por arriba, por abajo y por el lado)
    Collision::Contact crossbarContact = collideBallWith(goalP1.crossbar, E_CROSSBAR);
    // Si la pelota está encima (normal hacia arriba) y casi parada, la empujamos hacia el campo
    if (crossbarContact.isColliding && crossbarContact.normal.y < -0.5f && fabsf(ballVel.x) < 30.0f)
        ballVel.x = CROSSBAR_PUSH;

    crossbarContact = collideBallWith(goalP2.crossbar, E_CROSSBAR);
    if (crossbarContact.isColliding && crossbarContact.normal.y < -0.5f && fabsf(ballVel.x) < 30.0f)
        ballVel.x = -CROSSBAR_PUSH;

    // 7. Detección de Gol
    if (CheckCollisionPointRec(ballPos, goalP1.netArea)) {
        this->scoreP2++;
        this->goalBannerTimer = 1.5f;
        this->goalBannerText = "¡GOL DEL JUGADOR 2!";
        this->goalBannerColor = BLUE;
        this->ball.setVelocity(0.0f, 0.0f);
        std::cout << "¡GOL DEL JUGADOR 2! Marcador: " << scoreP1 << " - " << scoreP2 << std::endl;
        return;
    }

    if (CheckCollisionPointRec(ballPos, goalP2.netArea)) {
        this->scoreP1++;
        this->goalBannerTimer = 1.5f;
        this->goalBannerText = "¡GOL DEL JUGADOR 1!";
        this->goalBannerColor = RED;
        this->ball.setVelocity(0.0f, 0.0f);
        std::cout << "¡GOL DEL JUGADOR 1! Marcador: " << scoreP1 << " - " << scoreP2 << std::endl;
        return;
    }

    // 8. Colisiones Pelota - Jugadores (pasos del png: normal, velocidad relativa, rebote con e)
    collideBallWith(this->player1.body, E_PLAYER);
    collideBallWith(this->player2.body, E_PLAYER);
}

void MainGameState::render()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Suelo
    DrawRectangle(0, (int)GROUND_Y, (int)FIELD_WIDTH, 120, GREEN);
    DrawLine(0, (int)GROUND_Y, (int)FIELD_WIDTH, (int)GROUND_Y, DARKGREEN);

    // Porterías (Redes y Largueros)
    DrawRectangleRec(goalP1.netArea, LIGHTGRAY);
    DrawRectangleRec(goalP2.netArea, LIGHTGRAY);
    DrawRectangleRec(goalP1.crossbar.getRect(), DARKGRAY);
    DrawRectangleRec(goalP2.crossbar.getRect(), DARKGRAY);

    // Jugadores y Pelota
    DrawRectangleRec(player1.body.getRect(), RED);
    DrawRectangleRec(player2.body.getRect(), BLUE);
    DrawCircleV(ball.getPosition(), ball.getRadius(), BLACK);

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

    // Cartel temporal de ¡GOL!
    if (this->goalBannerTimer > 0.0f)
    {
        int bannerFontSize = 45;
        int bannerTextWidth = MeasureText(this->goalBannerText.c_str(), bannerFontSize);
        int bannerX = 540 - (bannerTextWidth / 2);
        int bannerY = 270;

        // Recuadro semitransparente con borde del color del goleador
        DrawRectangle(bannerX - 40, bannerY - 15, bannerTextWidth + 80, 75, Fade(BLACK, 0.75f));
        DrawRectangleLines(bannerX - 40, bannerY - 15, bannerTextWidth + 80, 75, this->goalBannerColor);

        // Texto del gol
        DrawText(this->goalBannerText.c_str(), bannerX, bannerY, bannerFontSize, this->goalBannerColor);
    }

    EndDrawing();
}
