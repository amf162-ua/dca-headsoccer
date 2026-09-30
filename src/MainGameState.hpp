#pragma once
#include <GameState.hpp>
extern "C" {
    #include <raylib.h>
}
#include "physics/physics.hpp"

struct Player {
    float x;
    float y;
    float vx;
    float vy;
};

struct Ball {
    float x;
    float y;
    float vx;
    float vy;
};

class MainGameState : public GameState
{
    private:
        Player player1;
        Player player2;
        Player npc;
        Ball ball;
        Physics physics;

    public:
        MainGameState();
        ~MainGameState() = default;

        void init() override;
        void handleInput() override;
        void update(float deltaTime) override;
        void render() override;

        void pause(){};
        void resume(){};

    
    private:
        char entered_key;
};