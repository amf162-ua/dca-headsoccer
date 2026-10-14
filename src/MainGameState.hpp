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
    bool isGrounded; //comprobamos si el jugador está tocando el suelo
};

struct Goal {
    Rectangle netArea;   // Área de la red
    Rectangle crossbar;  // Larguero superior
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

        Goal goalP1;
        Goal goalP2;

        int scoreP1;
        int scoreP2;

        float matchTimer; // Temporizador del partido

        void resetPositions();

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