#pragma once
#include <GameState.hpp>
#include <string>
extern "C" {
    #include <raylib.h>
}

class GameOverState : public GameState
{
    private:
        int finalScoreP1;
        int finalScoreP2;
        std::string winnerMessage;

    public:
        // El constructor recibe el marcador final del partido
        GameOverState(int score1, int score2);
        ~GameOverState() = default;

        void init() override;
        void handleInput() override;
        void update(float deltaTime) override;
        void render() override;

        void pause() override {};
        void resume() override {};
};