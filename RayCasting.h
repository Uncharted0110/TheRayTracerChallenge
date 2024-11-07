#ifndef RAYCASTING_H
#define RAYCASTING_H

#include "PointVec.h"
#include "ColorCanvas.h"
#include "Patterns.h"

#include <iostream>
#include <vector>
#include <string.h>

class Ray {
public:
	Entity origin, direction;

	Ray();
	Ray(Entity ori, Entity dir);
	
	void PrintRay();
};

class Material {
public:
	float ambient, diffuse, specular, shininess, reflective, transparency, refractive_index;
	Color color;
	Patterns* pattern = NULL;

	Material();
};

class Intersection;

class Shape {
public:
	std::vector<std::vector<float>> transform = {
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};
	Material mat;
	std::string ID;
	Shape* parent = NULL;

	virtual std::vector<Intersection> local_intersect(Ray localRay) = 0;
	virtual Entity local_normal_at(Entity point, Intersection hit) = 0;
	
	virtual bool includes(const Shape* other) {
		return this == other;
	}
};

class TestShape : public Shape {
public:
	Ray savedRay;

	std::vector<Intersection> local_intersect(Ray localRay);
	Entity local_normal_at(Entity point, Intersection hit);
};

class Sphere : public Shape {
public:
	std::vector<Intersection> local_intersect(Ray localRay);
	Entity local_normal_at(Entity point, Intersection hit);
};

class Plane : public Shape {
public:
	std::vector<Intersection> local_intersect(Ray localRay);
	Entity local_normal_at(Entity point, Intersection hit);
};

class Cube : public Shape {
public:
	std::vector<Intersection> local_intersect(Ray localRay);
	Entity local_normal_at(Entity point, Intersection hit);
};

class Cylinder : public Shape {
public:
	std::vector<Intersection> local_intersect(Ray localRay);
	Entity local_normal_at(Entity point, Intersection hit);

	float minimum = -INFINITY;
	float maximum = INFINITY;
	bool closed = false;
};

class Cone : public Shape {
public:
	std::vector<Intersection> local_intersect(Ray localRay);
	Entity local_normal_at(Entity point, Intersection hit);

	float minimum = -INFINITY;
	float maximum = INFINITY;
	bool closed = false;
};

class Group : public Shape {
public:
	std::vector<Intersection> local_intersect(Ray localRay);
	Entity local_normal_at(Entity point, Intersection hit);

	std::vector<Shape*> children;

	bool includes(const Shape* other) {
		for(const auto& child : children)
			if (child->includes(other)) {
				return true;
			}
		return false;
	}
};

class Triangle : public Shape {
public:
	std::vector<Intersection> local_intersect(Ray localRay);
	Entity local_normal_at(Entity point, Intersection hit);

	Entity p1, p2, p3, e1, e2, normal;
	Entity n1, n2, n3;

	bool smoothTriangle;

	Triangle();
	Triangle(Entity p1, Entity p2, Entity p3);
};

class CSG : public Shape {
public:
	std::vector<Intersection> local_intersect(Ray localRay);
	Entity local_normal_at(Entity point, Intersection hit);

	std::string operation;
	Shape* left;
	Shape* right;
	
	CSG(std::string operation, Shape* left, Shape* right);

	bool includes(const Shape* other) {
		return left && left->includes(other) || right && right->includes(other);
	}
};

class Intersection {
public:
	float t;
	Shape* s;
	float u = 0, v = 0;

	Intersection();
	Intersection(float t, Shape* s);
};

Entity Position_At_t(Ray r, float t);
std::vector<Intersection> Multiple_Intersections(std::vector<Intersection> list);
Intersection Hit(std::vector<Intersection> list);
Ray Transform(Ray r, std::vector<std::vector<float>> transformMat);
void Set_Transform(Shape& s, std::vector<std::vector<float>> transformMat);
bool compare_tval(Intersection& a, Intersection& b);
std::vector<Intersection> Intersect(Shape& s, Ray r);
Sphere CreateSphere();
Plane CreatePlane();
Color PatternAtShape(Patterns& pattern, Shape& s, Entity point);
std::array<float, 2> CheckAxis(float origin, float direction, float min, float max);
bool CheckCapCylinder(Ray ray, float t);
void IntersectCapsCylinder(Cylinder& cyl, Ray ray, std::vector<Intersection>& xs);
bool CheckCapCone(Ray ray, float t, float y);
void IntersectCapsCone(Cone& cone, Ray ray, std::vector<Intersection>& xs);
void AddChild(Group* g, Shape* s);
Entity worldToObject(Shape* s, Entity point);
Entity NormalToWorld(Shape* s, Entity normal);
Triangle* Smooth_Triangle(Entity p1, Entity p2, Entity p3, Entity n1, Entity n2, Entity n3);
Intersection Intersection_with_uv(float t, Triangle* shape, float u, float v);
bool IntersectionAllowed(std::string op, bool lhit, bool inl, bool inr);
std::vector<Intersection> FilterIntersection(CSG* c, std::vector<Intersection> xs);

#endif // !RAYCASTING_H
