#include "body.hpp"



//Setters
void Body::setBodyType(bodyType t)
{
    type = t;
    setMass(mass); // Recalcula invMass: STATIC/ANIMATED -> 0
}

void Body::setRectangleBody(float w, float h)
{
    shape = bodyShape::RECTANGLE;
    width = w;
    height = h;
    radius = 0.0f;
}

void Body::setCircleBody(float r)
{
    shape = bodyShape::CIRCLE;
    radius = r;
    width = height = r * 2.0f; // Así getRect() también vale para el círculo
}

/**
 * Establece la masa del cuerpo.
 * @param m La masa del cuerpo.
 */
void Body::setMass(float m)
{
    mass = m;
    if (m > 0.0f && type == bodyType::DYNAMIC)
        invMass = 1.0f / m;
    else
        invMass = 0.0f;
}

void Body::setPosition(float x, float y)
{
    position = { x, y };
}

void Body::setVelocity(float vx, float vy)
{
    velocity = { vx, vy };
}

//Getters

Body::bodyType Body::getBodyType() const { return type; }
Body::bodyShape Body::getBodyShape() const { return shape; }

Vector2& Body::getPosition() { return position; }
Vector2& Body::getVelocity() { return velocity; }
const Vector2& Body::getPosition() const { return position; }
const Vector2& Body::getVelocity() const { return velocity; }

float Body::getMass() const { return mass; }
float Body::getInvMass() const { return invMass; }
float Body::getWidth() const { return width; }
float Body::getHeight() const { return height; }
float Body::getRadius() const { return radius; }

Rectangle Body::getRect() const
{
    return { position.x - width / 2.0f, position.y - height / 2.0f, width, height };
}
