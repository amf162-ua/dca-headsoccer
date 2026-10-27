#pragma once
#include <GameState.hpp>
extern "C" {
    #include <raylib.h>
}
#include "physics/physics.hpp"
#include "physics/body.hpp"
#include "physics/collision.hpp"
#include "PowerUp.hpp"
#include <string>

struct Player {
    Body body;       // Posicion (CENTRO), velocidad (px/s) y caja del jugador
    bool isGrounded; //comprobamos si el jugador está tocando el suelo
    float kickTimer;     // Tiempo que dura el pie extendido (ej: 0.18s)
    float kickCooldown;  // Tiempo antes de poder volver a chutar (ej: 0.35s)
    bool isFacingRight;  // true para J1 (mira a dcha), false para J2 (mira a izq)

    Rectangle getFootRect() const {
        Rectangle bodyRect = body.getRect();
        float footWidth = 24.0f;
        float footHeight = 14.0f;

        // El pie siempre está a ras del suelo (en la base del cuerpo)
        float footY = bodyRect.y + bodyRect.height - footHeight;

        // Si estamos chutando, el pie se estira 22 píxeles hacia adelante
        float extension = (kickTimer > 0.0f) ? 22.0f : 0.0f;

        // Posición base del pie: en la esquina delantera del jugador
        // J1 (isFacingRight): esquina delantera derecha + extensión
        // J2 (!isFacingRight): esquina delantera izquierda - extensión
        float footX = isFacingRight
            ? (bodyRect.x + bodyRect.width - 12.0f + extension)
            : (bodyRect.x - footWidth + 12.0f - extension);

        return { footX, footY, footWidth, footHeight };
    }

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
        PowerUp powerUp;

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
        void updateKickTimers(Player& player, float deltaTime);
        bool kickBall(Player& player, Body& targetBall);
        void renderDebug();
        void renderBallTrajectory();
        void renderPowerUp();

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
