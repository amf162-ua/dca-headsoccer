#pragma once

#include "physics/body.hpp"

class PowerUp
{
    public:
        PowerUp();

        void reset();
        void update(float deltaTime);
        bool tryCollect(const Body& player);

        bool isAvailable() const;
        Vector2 getPosition() const;
        float getRadius() const;
        float getRemainingTime() const;
        float getBallSpeedFactor() const;
        float getBallDeltaTime(float deltaTime) const;

    private:
        Vector2 position;
        float radius;
        bool available;
        float respawnTimer;
        float effectTimer;
};
