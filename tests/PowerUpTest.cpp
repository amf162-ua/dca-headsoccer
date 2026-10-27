#include "PowerUp.hpp"
#include "physics/PhysicsGeometry.hpp"
#include "physics/physics.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

bool approximatelyEqual(float actual, float expected)
{
    return std::fabs(actual - expected) < 0.0001f;
}

Body createPlayer(float x, float y)
{
    Body player;
    player.setRectangleBody(PhysicsGeometry::playerWidth, PhysicsGeometry::playerHeight);
    player.setBodyType(Body::bodyType::ANIMATED);
    player.setPosition(x, y);
    return player;
}

void testCollection()
{
    PowerUp powerUp;
    Vector2 position = powerUp.getPosition();
    float halfWidth = PhysicsGeometry::playerWidth / 2.0f;
    float halfHeight = PhysicsGeometry::playerHeight / 2.0f;

    assert(powerUp.isAvailable());
    assert(approximatelyEqual(position.x, PhysicsGeometry::fieldWidth / 2.0f));
    assert(position.y + powerUp.getRadius() < PhysicsGeometry::groundY - PhysicsGeometry::playerHeight);
    assert(approximatelyEqual(powerUp.getBallDeltaTime(1.0f), 1.0f));

    Body player1 = createPlayer(125.0f, 560.0f);
    assert(!powerUp.tryCollect(player1));

    // La esquina de la caja del objeto no pertenece al circulo.
    player1.setPosition(position.x + halfWidth + 17.0f, position.y - halfHeight - 17.0f);
    assert(!powerUp.tryCollect(player1));

    // El contacto con el borde del circulo tambien debe recoger el objeto.
    player1.setPosition(position.x - powerUp.getRadius() - halfWidth, position.y);
    assert(powerUp.tryCollect(player1));
    assert(!powerUp.isAvailable());
    assert(approximatelyEqual(powerUp.getRemainingTime(), 12.0f));
    assert(approximatelyEqual(powerUp.getBallSpeedFactor(), 0.7f));

    // El segundo jugador no puede recoger de nuevo un objeto que ya desaparecio.
    Body player2 = createPlayer(position.x, position.y);
    powerUp.update(1.0f);
    assert(!powerUp.tryCollect(player2));
    assert(approximatelyEqual(powerUp.getRemainingTime(), 11.0f));

    powerUp.reset();
    assert(powerUp.tryCollect(player2));
}

void testJumpCollection()
{
    PowerUp powerUp;
    float standingY = PhysicsGeometry::groundY - PhysicsGeometry::playerHeight / 2.0f;
    Body player = createPlayer(powerUp.getPosition().x, standingY);
    assert(!powerUp.tryCollect(player));

    player.setVelocity(0.0f, -420.0f);
    Physics physics;
    bool collected = false;
    for (int frame = 0; frame < 60 && !collected; frame++)
    {
        float deltaTime = 1.0f / 60.0f;
        physics.Gravity(player.getVelocity().y, deltaTime);
        physics.Integrate(player, deltaTime);
        collected = powerUp.tryCollect(player);
    }
    assert(collected);
}

void testDurationAndRespawn()
{
    PowerUp powerUp;
    Body player = createPlayer(powerUp.getPosition().x, powerUp.getPosition().y);
    assert(powerUp.tryCollect(player));

    powerUp.update(11.75f);
    assert(approximatelyEqual(powerUp.getRemainingTime(), 0.25f));
    assert(!powerUp.isAvailable());

    // Un fotograma que cruza el final del efecto combina tiempo lento y normal.
    assert(approximatelyEqual(powerUp.getBallDeltaTime(0.5f), 0.425f));
    powerUp.update(0.25f);
    assert(approximatelyEqual(powerUp.getRemainingTime(), 0.0f));
    assert(approximatelyEqual(powerUp.getBallSpeedFactor(), 1.0f));
    assert(approximatelyEqual(powerUp.getBallDeltaTime(0.5f), 0.5f));
    assert(powerUp.isAvailable());
    assert(powerUp.tryCollect(player));
    assert(approximatelyEqual(powerUp.getRemainingTime(), 12.0f));

    // Un saque o una partida nueva cancela el efecto y recupera el objeto.
    powerUp.reset();
    assert(powerUp.isAvailable());
    assert(approximatelyEqual(powerUp.getRemainingTime(), 0.0f));
    assert(approximatelyEqual(powerUp.getBallDeltaTime(1.0f), 1.0f));
}

void testBallMovement()
{
    PowerUp powerUp;
    Body player = createPlayer(powerUp.getPosition().x, powerUp.getPosition().y);
    assert(powerUp.tryCollect(player));

    Body ball;
    ball.setCircleBody(PhysicsGeometry::ballRadius);
    ball.setPosition(0.0f, 0.0f);
    ball.setVelocity(300.0f, -200.0f);

    Physics physics;
    for (int frame = 0; frame < 60; frame++)
    {
        float deltaTime = 1.0f / 60.0f;
        physics.Integrate(ball, powerUp.getBallDeltaTime(deltaTime));
        powerUp.update(deltaTime);
    }

    assert(approximatelyEqual(ball.getPosition().x, 210.0f));
    assert(approximatelyEqual(ball.getPosition().y, -140.0f));
    assert(approximatelyEqual(ball.getVelocity().x, 300.0f));
    assert(approximatelyEqual(ball.getVelocity().y, -200.0f));
}

int main()
{
    testCollection();
    testJumpCollection();
    testDurationAndRespawn();
    testBallMovement();
    std::cout << "Pruebas del power-up superadas." << std::endl;
    return 0;
}
