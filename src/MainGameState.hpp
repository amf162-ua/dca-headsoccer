#pragma once
#include <GameState.hpp>
extern "C" {
    #include <raylib.h>
}
#include "physics/physics.hpp"
#include "physics/body.hpp"
#include "physics/collision.hpp"
#include <string>

struct Player {
    Body body;       // Posicion (CENTRO), velocidad (px/s) y caja del jugador
    bool isGrounded; //comprobamos si el jugador está tocando el suelo
};

struct Goal {
    Rectangle netArea;   // Área de la red
    Body crossbar;       // Larguero superior (cuerpo STATIC para que la pelota rebote con las fisicas)
};

class MainGameState : public GameState
{
    private:
        Player player1;
        Player player2;
        Player npc;
        Body ball;           // Pelota: cuerpo circular DYNAMIC
        Physics physics;

        Goal goalP1;
        Goal goalP2;

        int scoreP1;
        int scoreP2;

        float matchTimer; // Temporizador del partido

        float goalBannerTimer;       // Temporizador para el cartel de gol
        std::string goalBannerText;  // Texto a mostrar ("¡GOL DEL JUGADOR 1!", etc.)
        Color goalBannerColor;       // Color del cartel
        bool debugEnabled;
        bool trajectoryEnabled;
        float lastDeltaTime;

        void resetPositions();
        void keepPlayerInField(Player& player);                                // Suelo y limites de pantalla
        Collision::Contact collideBallWith(Body& circle, Body& other, float restitution);
        void renderDebug();
        void renderBallTrajectory();

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
