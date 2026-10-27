#include "PowerUp.hpp"
#include "physics/PhysicsGeometry.hpp"

namespace
{
    const float POWER_UP_RADIUS = 18.0f;
    const float RESPAWN_TIME = 12.0f;
    const float EFFECT_DURATION = 6.0f;
    const float BALL_SPEED_FACTOR = 0.7f;
}

PowerUp::PowerUp()
{
    reset();
}

void PowerUp::reset()
{
    radius = POWER_UP_RADIUS;
    position = {PhysicsGeometry::fieldWidth / 2.0f, PhysicsGeometry::groundY - radius};
    available = true;
    respawnTimer = 0.0f;
    effectTimer = 0.0f;
}

void PowerUp::update(float deltaTime)
{
    if (effectTimer > 0.0f)
    {
        effectTimer -= deltaTime;
        if (effectTimer < 0.0f)
        {
            effectTimer = 0.0f;
        }
    }

    if (!available)
    {
        respawnTimer -= deltaTime;
        if (respawnTimer <= 0.0f)
        {
            respawnTimer = 0.0f;
            available = true;
        }
    }
}

bool PowerUp::tryCollect(const Body& player)
{
    if (!available || !CheckCollisionCircleRec(position, radius, player.getRect()))
    {
        return false;
    }

    available = false;
    respawnTimer = RESPAWN_TIME;
    effectTimer = EFFECT_DURATION;
    return true;
}

bool PowerUp::isAvailable() const
{
    return available;
}

Vector2 PowerUp::getPosition() const
{
    return position;
}

float PowerUp::getRadius() const
{
    return radius;
}

float PowerUp::getRemainingTime() const
{
    return effectTimer;
}

float PowerUp::getBallSpeedFactor() const
{
    if (effectTimer > 0.0f)
    {
        return BALL_SPEED_FACTOR;
    }
    return 1.0f;
}

float PowerUp::getBallDeltaTime(float deltaTime) const
{
    float slowedTime = deltaTime;
    if (slowedTime > effectTimer)
    {
        slowedTime = effectTimer;
    }

    // Si el efecto termina en este fotograma, el resto del tiempo es normal.
    return slowedTime * BALL_SPEED_FACTOR + (deltaTime - slowedTime);
}
