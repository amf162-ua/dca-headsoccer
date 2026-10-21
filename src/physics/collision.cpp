#include "collision.hpp"
#include <raymath.h>
#include <cmath>

namespace Collision{
    Contact detectCircleAABB(const Body& circle, const Body& rectangle){
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




    /*
        Resolver la colisión entre un circulo y un rectangulo.

        !!!!!!!! COMPROBAR DESPUES DE IMPLEMENTAR Y PROBAR !!!!!!!!!!
        No creo que haga falta una fucnión nueva para choque jugador-jugador, solo habría que añadir
        algunas condiciones para separar jugador de circulo y aplicar reglas parecidas o iguales.
        No hace falta que tenga en cuenta la gravedad, ya que aunq sea un vector de fuerza mas, ni
        la masa ni la gravedad afectan a la resolución de la colisión, solo a la aceleración y velocidad.

        para descartar circle y square:
        if (circle.getBodyShape() != Body::bodyShape::CIRCLE) return; ???????????????????? por si acaso

        y tambien alomejor la pelota si se queda en la cabeza del jugador, hace
        tembleques raros por estar colisionando constantemente y no se verá nada fluido,
        si passa supongo que con poner e = 0 cuando vn sea pequeño, o algo asi, se soluciona.
    */
    void resolveContact(Body& circle, Body& rectangle, const Contact& contact, float e){
        if (!contact.isColliding){ return;} // No hay colisión, no hacemos nada
        if (contact.penetration <= 0){ return;} // No hay penetración, no hacemos nada

        // Usamos invMass (1/masa) y no 1/getMass(): si el cuerpo es STATIC o ANIMATED, invMass = 0,
        // que es lo mismo que masa infinita -> no se mueve. Asi tampoco dividimos nunca entre 0.
        float invMassCircle = circle.getInvMass();
        float invMassRect   = rectangle.getInvMass();
        float invMassSum    = invMassCircle + invMassRect;
        if (invMassSum == 0.0f){ return;} // Los dos son inamovibles, no hay nada que resolver

        /*  ---- 1. Sacar la pelota del jugador (corrección de posición) ----
            Como el juego va por frames, cuando detectamos el choque la pelota YA se ha metido
            "penetration" px dentro del jugador, que es el problema que nos comenttó el profe
            en clase.
             Si solo cambiamos la velocidad:
              - La pelota sigue dentro, y el frame siguiente se vuelve a detectar el choque.
              - Se queda "pegada" o temblando dentro del jugador, o la atraviesa.
            Solucion: moverla hacia fuera "penetration" px en la direccion de la normal
            (la normal apunta del jugador a la pelota), y asi queda justo tocando el borde.

            Es como si la pelota se hubiera quedado pegada al jugador, y ahora la soltamos hacia fuera.
            Si el cuerpo es STATIC, invMassRect = 0, asi que solo se mueve la pelota.
            Si el cuerpo es DYNAMIC, invMassRect > 0, asi que se mueven los dos.

            Si los dos se pueden mover, el empujon se reparte segun la masa: el ligero se mueve mas.
            Con el jugador (invMassRect = 0), la pelota se lleva el 100% del empujon.
        */
        float circleShare = invMassCircle / invMassSum; // Fraccion que se mueve la pelota (0..1)
        float rectShare   = invMassRect   / invMassSum; // Fraccion que se mueve el jugador

        circle.getPosition().x    += contact.normal.x * contact.penetration * circleShare;
        circle.getPosition().y    += contact.normal.y * contact.penetration * circleShare;
        rectangle.getPosition().x -= contact.normal.x * contact.penetration * rectShare; // Al reves: la normal apunta hacia la pelota
        rectangle.getPosition().y -= contact.normal.y * contact.penetration * rectShare;

        // ---- 2. Velocidad relativa (paso 4 del png) ----
        Vector2 relativeVelocity = { circle.getVelocity().x - rectangle.getVelocity().x,   //Que es la diferencia de v entre los dos cuerpos, por si se separan o acercan
                                      circle.getVelocity().y - rectangle.getVelocity().y };

        // ---- 3. Velocidad por la normal de colisión (paso 5 del png) ----
        float velocityAlongNormal = relativeVelocity.x * contact.normal.x + relativeVelocity.y * contact.normal.y;

        // Si la velocidad a lo largo de la normal es positiva, significa que se están separando
        // (ya hemos corregido la posicion, pero la velocidad no se toca)
        if (velocityAlongNormal > 0) return;

        // ---- 4. Impulso (pasos 6 y 7 del png) ----
        // j = cuanto "golpe" se dan. Se reparte segun las masas: a mas ligero, más impulso y velocidad :).
        // Con invMassRect = 0 queda exactamente la formula del png: v' = v - (1+e)*vn*n
        float j = -(1 + e) * velocityAlongNormal;
        j = j / invMassSum;

        // Aplicar el impulso al círculo y al rectángulo (cada uno en un sentido)
        Vector2 impulse = { j * contact.normal.x, j * contact.normal.y };
        circle.getVelocity().x    += impulse.x * invMassCircle;
        circle.getVelocity().y    += impulse.y * invMassCircle;
        rectangle.getVelocity().x -= impulse.x * invMassRect;
        rectangle.getVelocity().y -= impulse.y * invMassRect;

        // SACABo
    }
}
