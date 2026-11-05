#pragma once
#include <GameState.hpp>
extern "C" {
    #include <raylib.h>
}

class MenuState : public GameState
{
    public:
        MenuState();
        ~MenuState() = default;

        void init() override;
        void handleInput() override;
        void update(float deltaTime) override;
        void render() override;

        void pause() override {};
        void resume() override {};
};