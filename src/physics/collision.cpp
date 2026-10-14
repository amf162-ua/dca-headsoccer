#include "collision.hpp"
#include <raymath.h>
#include <cmath>

namespace Collision{
    Contact detectCircleAABB(const Body& circle, const Body& rectangle)
    {
        Contact result{}; // Empieza sin colisión: isColliding = false, normal = (0,0), penetration = 0

        // Obtener la posición del círculo y el rectángulo
        Vector2 circlePos = circle.getPosition();
        Vector2 rectPos = rectangle.getPosition();
        float radius = circle.getRadius();

        // Obtener las dimensiones del rectángulo
        float rectHalfWidth = rectangle.getWidth() / 2.0f; // Divido entre dos para calcular la distancia de las paredes del centro.
        float rectHalfHeight = rectangle.getHeight() / 2.0f;

        // Calcular los límites del rectángulo (Aqui estandarizamos ya las posiciiones de los bordes del rectangulo)
        float rectLeft = rectPos.x - rectHalfWidth;
        float rectRight = rectPos.x + rectHalfWidth;
        float rectTop = rectPos.y - rectHalfHeight;
        float rectBottom = rectPos.y + rectHalfHeight;

        // Encontrar el punto más cercano en el rectángulo al centro del círculo
        // Clamp es para que si el centro del circulo esta fuera del rectangulo, se quede en el borde del rectangulo.
        float closestX = Clamp(circlePos.x, rectLeft, rectRight);  // Punto de izq o dch mas cercano al centro del circulo desde el rectangulo
        float closestY = Clamp(circlePos.y, rectTop, rectBottom);  // Punto de arr o abj mas cercano al centro del circulo desde el rectangulo
        Vector2 Q = { closestX, closestY }; // Coordenada mas cercana del rectangulo al centro del circulo


        // Ahora ya tenemos (x,y) del rectangulo mas cercano al (0,0) relativo del circulo :D
        // En el png es Q

        // Calcular la distancia entre el centro del círculo y el punto más cercano
        float distanceX = circlePos.x - Q.x;
        float distanceY = circlePos.y - Q.y;

        // Vector del rectangulo al circulo, en el png es "d" (todavia NO es la normal: no mide 1)
        Vector2 d = { distanceX, distanceY };

        // Calcular la distancia al cuadrado, |d|^2
        float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

        // Si |d|^2 >= r^2, la pelota no llega a tocar la caja
        if (distanceSquared >= radius * radius) {
            return result; // isColliding sigue a false
        }

        /*  Caso raro: el centro del circulo esta ^dentro del rectangulo 
            Clamp no ha recortado nada, asi que Q == centro y d = (0,0).
            Un vector de longitud 0 no tiene direccion -> no se puede normalizar (0/0 = NaN).
        */ 
        if (distanceSquared == 0.0f) {
            float toLeft   = circlePos.x - rectLeft;    // Distancia del centro a cada cara
            float toRight  = rectRight - circlePos.x;
            float toTop    = circlePos.y - rectTop;
            float toBottom = rectBottom - circlePos.y;

            // Empezamos suponiendo que la mas cercana es la izquierda y vamos comparando
            float minDist = toLeft;
            result.normal = { -1, 0 };                                              // Empujar hacia la izquierda
            if (toRight  < minDist) { minDist = toRight;  result.normal = { 1, 0 }; }  // Hacia la derecha
            if (toTop    < minDist) { minDist = toTop;    result.normal = { 0, -1 }; } // Hacia arriba (y negativa en raylib)
            if (toBottom < minDist) { minDist = toBottom; result.normal = { 0, 1 }; }  // Hacia abajo

            // Para que la pelota quede justo tocando la cara: tiene que recorrer
            // lo que le falta al centro para salir (minDist) + su radio entero.
            result.penetration = minDist + radius;
            result.isColliding = true;
            return result;
        }

        // ---- Caso normal: el centro esta fuera, pero la pelota se mete ----
        float distance = sqrtf(distanceSquared); // |d|, la distancia real (ahora si hace falta la raiz)

        // Normalizar: dividir d entre su longitud -> misma direccion, pero mide 1.
        // Ejemplo: d = (3, -4), distance = 5  ->  normal = (0.6, -0.8)
        result.normal = { d.x / distance, d.y / distance };

        // La pelota deberia estar a "radius" de Q, pero esta a "distance".
        // Lo que falta es lo que se ha metido dentro.
        result.penetration = radius - distance;
        result.isColliding = true;
        return result;
    }
}
