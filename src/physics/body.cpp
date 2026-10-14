#include "body.hpp"


//Constructor
Body::Body(){
    type(bodyType::DYNAMIC), 
    shape(bodyShape::RECTANGLE),
    position{0.0f, 0.0f}, 
    velocity{0.0f, 0.0f},
    mass(1.0f), 
    invMass(1.0f),
    width(0.0f), 
    height(0.0f), 
    radius(0.0f)
}

/*
    Constructor de la clase Body.
    @param type El tipo de cuerpo.      -> DYNAMIC, STATIC o ANIMATED (Si no es dynamic invMass = 0)
    @param shape La forma del cuerpo.   -> CIRCLE o RECTANGLE (Si es CIRCLE, width y height se calculan a partir del radio)
    @param pos La posición del cuerpo.
    @param vel La velocidad del cuerpo.
    @param mass La masa del cuerpo.
    @param width El ancho del cuerpo.
    @param height La altura del cuerpo.
    @param radius El radio del cuerpo.
*/
Body::Body(bodyType type, bodyShape shape, Vector2 pos, Vector2 vel, 
                float mass, float width, float height, float radius){
    this->type = type;
    this->shape = shape;
    this->position = pos;
    this->velocity = vel; // En un principio, la velocidad inicial es 0, pero se puede cambiar después con setVelocity()
    this->mass = mass;
    // Si raduis tine valor, se calcula el ancho y alto a partir del radio, en caso de que pongamos de todo xd
    if (radius != NULL) {
        this->radius = radius;
        this->width = radius * 2.0f;
        this->height = radius * 2.0f;
    } else {
        this->width = width;
        this->height = height;
        this->radius = 0.0f; // Si no es un círculo, el radio es 0
    }
    // Calcula invMass: si es STATIC o ANIMATED, invMass = 0
    if (mass > 0.0f && type == bodyType::DYNAMIC)
        this->invMass = 1.0f / mass;
    else
        this->invMass = 0.0f;
}

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
