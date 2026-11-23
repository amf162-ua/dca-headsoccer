#pragma once
#include <GameState.hpp>
#include <vector>
extern "C" {
    #include <raylib.h>
}

class MenuState : public GameState
{
    private:
        int selectedMap;
        float carouselPosition;

        // Miniaturas de los estadios que tienen sprites (vacías si no tienen)
        std::vector<Texture2D> previewBackgrounds;
        std::vector<Texture2D> previewGrass;

        void drawMapCard(int mapIndex, float position) const;
        void drawMapPreview(Rectangle area, int mapIndex) const;

    public:
        MenuState();
        ~MenuState() override;

        void init() override;
        void handleInput() override;
        void update(float deltaTime) override;
        void render() override;

        void pause() override {};
        void resume() override {};
};
