#include <iostream>
#include <cmath>
#include "PointVec.h"

#define EPSILON 0.00001

Entity::Entity() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

Entity::Entity(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

Entity CreateVector(float x, float y, float z) {
    return Entity(x, y, z, 0.0f);
}

Entity CreatePoint(float x, float y, float z) {
    return Entity(x, y, z, 1.0f);
}

Entity Entity::operator+(const Entity& other) const {
    return Entity(x + other.x, y + other.y, z + other.z, w + other.w);
}

Entity Entity::operator-(const Entity& other) const {
    return Entity(x - other.x, y - other.y, z - other.z, w - other.w);
}

Entity Entity::operator*(float scalar) const {
    return Entity(x * scalar, y * scalar, z * scalar, w * scalar);
}

void PrintEntity(Entity e) {
    std::cout << e.x << " " << e.y << " " << e.z << std::endl;
}

void TypeofEntity(Entity e) {
    if (e.w == 1.0f)
        std::cout << "Point" << std::endl;
    else
        std::cout << "Vector" << std::endl;
}

bool equal(float a, float b) {
    return abs(a - b) < EPSILON;
}

Entity Add(Entity a, Entity b) {
    if (a.w + b.w != 2) {
        return Entity(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
    }
    else {
        std::cout << "Cannot add two points" << std::endl;
        return Entity(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

Entity Subtract(Entity a, Entity b) {
    if (a.w - b.w >= 0) {
        return Entity(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
    }
    else {
        std::cout << "Cannot subtract point from vector" << std::endl;
        return Entity(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

Entity Negate(Entity a) {
    return Entity(-a.x, -a.y, -a.z, -a.w);
}

Entity Multiply(Entity a, float scalar) {
    return Entity(a.x * scalar, a.y * scalar, a.z * scalar, a.w * scalar);
}

Entity Divide(Entity a, float scalar) {
    return Entity(a.x / scalar, a.y / scalar, a.z / scalar, a.w / scalar);
}

float Magnitude(Entity a) {
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

Entity Normalize(Entity a) {
    float mag = Magnitude(a);
    if (mag == 0) return a;
    return Entity(a.x / mag, a.y / mag, a.z / mag, 0.0f);
}

float DotProduct(Entity vecA, Entity vecB) {
    return vecA.x * vecB.x + vecA.y * vecB.y + vecA.z * vecB.z;
}

Entity CrossProduct(Entity vecA, Entity vecB) {
    return CreateVector(vecA.y * vecB.z - vecA.z * vecB.y,
        vecA.z * vecB.x - vecA.x * vecB.z,
        vecA.x * vecB.y - vecA.y * vecB.x);
}

Projectile::Projectile(Entity pos, Entity vel) {
    position = CreatePoint(pos.x, pos.y, pos.z);
    velocity = CreateVector(vel.x, vel.y, vel.z);
}

Environment::Environment(Entity grav, Entity win) {
    gravity = CreateVector(grav.x, grav.y , grav.z);
    wind = CreateVector(win.x, win.y, win.z);
}

Projectile tick(Environment env, Projectile proj) {
    Entity pos = Add(proj.position, proj.velocity);
    Entity vel = Add(Add(proj.velocity, env.gravity), env.wind);

    return Projectile(pos, vel);
}
