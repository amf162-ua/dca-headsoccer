#pragma once

#include <vector>

extern "C" {
    #include <raylib.h>
}

// Público animado de las gradas.
// Cada aficionado (sprite de 10x15 px) se dibuja con un fotograma del sprite sheet (sentado, animando,
// de pie o con los brazos arriba) y una camiseta tintada con su propio color.
class Crowd
{
    public:
        Crowd() = default;
        ~Crowd();

        Crowd(const Crowd&) = delete;            // Tiene texturas propias: no se copia
        Crowd& operator=(const Crowd&) = delete;

        // Coloca aficionados en las zonas de grada (coordenadas de pantalla).
        // Si no hay zonas, el estadio se queda sin público y no se carga nada.
        void init(const Rectangle* zones, int zoneCount, const Color* shirtColors, int colorCount);

        // celebrating = true mientras se muestra el cartel de gol
        void update(float deltaTime, bool celebrating);
        void render() const;

    private:
        struct Spectator
        {
            Vector2 position; // Esquina superior izquierda del sprite
            Color shirt;
            float phase;      // Desfase para que no se muevan todos a la vez
            float excitement; // 0..1: cuánto se anima este aficionado
        };

        std::vector<Spectator> spectators;
        Texture2D baseTexture{};
        Texture2D shirtTexture{};
        float time = 0.0f;
        float celebrationTime = 0.0f; // Tiempo que lleva celebrando un gol
        bool celebrating = false;
        float waveTimer = 0.0f;       // Cuenta atrás para la próxima ola
        float waveX = -1.0f;          // Posición actual de la ola (-1 = sin ola)

        int frameFor(const Spectator& spectator, float& offsetY) const;
};
