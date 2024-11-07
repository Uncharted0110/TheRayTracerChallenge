#ifndef POINTVEC_H
#define POINTVEC_H

class Entity {
public:
    float x, y, z, w;

    Entity();
    Entity(float x, float y, float z, float w);

    Entity operator+(const Entity& other) const;
    Entity operator-(const Entity& other) const;
    Entity operator*(float scalar) const;
};

Entity CreateVector(float x, float y, float z);
Entity CreatePoint(float x, float y, float z);
bool equal(float a, float b);
Entity Add(Entity a, Entity b);
Entity Subtract(Entity a, Entity b);
Entity Negate(Entity a);
Entity Multiply(Entity a, float scalar);
Entity Divide(Entity a, float scalar);
float Magnitude(Entity a);
Entity Normalize(Entity a);
float DotProduct(Entity vecA, Entity vecB);
Entity CrossProduct(Entity vecA, Entity vecB);
void PrintEntity(Entity e);
void TypeofEntity(Entity e);

class Projectile {
public:
    Entity position;
    Entity velocity;

    Projectile(Entity pos, Entity vel);
};

class Environment {
public:
    Entity gravity;
    Entity wind;

    Environment(Entity grav, Entity win);
};

Projectile tick(Environment env, Projectile proj);

#endif
