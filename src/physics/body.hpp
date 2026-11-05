#pragma once
extern "C" {
    #include <raylib.h>
}

/**
    Clase que representa un cuerpo físico en el juego. Asi normalizamos todo como una clase.
    La posición es siempre el CENTRO del cuerpo.
*/
class Body{

    public:
        /**
            Tipos de cuerpos y tipos de formas que puede tener un cuerpo físico.
            bodyType: Si le afectan las fisicas o se mueve
            bodyShape: Forma del cuerpo (para colisiones y renderizado)
        */
        enum class bodyType{
            DYNAMIC, // Cuerpo dinámico, afectado por fuerzas y colisiones.
            STATIC,  // Cuerpo estático, no se mueve y no es afectado por fuerzas.
            ANIMATED // Cuerpo animado, se mueve pero no le afectan las fisicas (Lo llaman kinematic por ahi)
        };
        enum class bodyShape{
            CIRCLE, // Cuerpo con forma de círculo.
            RECTANGLE // Cuerpo con forma de rectángulo.
        };

        Body() = default;

        //Setters
        void setBodyType(bodyType type); // Si no es DYNAMIC, invMass pasa a 0.

        void setRectangleBody(float width, float height);
        void setCircleBody(float radius);

        void setMass(float mass); // Calcula también invMass, que sirve para calcular la aceleración.
        void setPosition(float x, float y);
        void setVelocity(float vx, float vy);

        //Getters
        bodyType getBodyType() const;
        bodyShape getBodyShape() const;

        Vector2& getPosition();             // Por referencia: se puede modificar (body.getVelocity().y += g * dt).
        Vector2& getVelocity();
        const Vector2& getPosition() const; // Solo lectura.
        const Vector2& getVelocity() const;

        float getMass() const;
        float getInvMass() const;
        float getWidth() const;
        float getHeight() const;
        float getRadius() const;

        Rectangle getRect() const; // Caja del cuerpo con esquina superior izquierda, para raylib.

    private:
        bodyType type = bodyType::DYNAMIC;      // Tipo de cuerpo físico.
        bodyShape shape = bodyShape::RECTANGLE; // Forma del cuerpo físico.

        Vector2 position{}; // CENTRO del cuerpo (px).
        Vector2 velocity{}; // Velocidad del cuerpo (px/s).

        float mass = 1.0f;    // Masa del cuerpo.
        float invMass = 1.0f; // 1/mass (0 si es STATIC o ANIMATED).

        float width = 0.0f;  // Ancho del cuerpo (para cuerpos rectangulares).   |  0 si no lo es
        float height = 0.0f; // Altura del cuerpo (para cuerpos rectangulares).  |  0 si no lo es

        float radius = 0.0f; // Radio del cuerpo (para cuerpos circulares).      |  0 si no lo es
};
