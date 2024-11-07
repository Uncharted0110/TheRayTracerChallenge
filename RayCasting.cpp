#include <iostream>
#include <random>
#include <vector>
#include <initializer_list>
#include <vector>
#include <array>
#include <utility>
#include <algorithm>

#include "RayCasting.h"
#include "PointVec.h"
#include "Matrices.h"
#include "Bounds.h"

#define EPSILON 0.00001

Ray::Ray(Entity ori, Entity dir) : origin(ori), direction(dir) {}

Ray::Ray(){}

void Ray::PrintRay() {
	std::cout << "Origin : ";
	PrintEntity(origin);

	std::cout << "Direction: ";
	PrintEntity(direction);
}

Entity Position_At_t(Ray r, float t) {
	return Add(r.origin, Multiply(r.direction, t));
}

Material::Material() : ambient(0.1f), diffuse(0.9f), specular(0.9f), shininess(200.0f), color(Color(1.0f, 1.0f, 1.0f)), 
		reflective(0.0f), transparency(0.0f), refractive_index(1.0f) {}


Sphere CreateSphere() {
	Sphere s = Sphere();
	s.transform = {
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};

	return s;
}

Plane CreatePlane() {
	return Plane();
}

Intersection::Intersection(){}

Intersection::Intersection(float t, Shape* s) : t(t), s(s) {}

bool compare_tval(Intersection& a, Intersection& b) {
	return a.t < b.t;
}

std::vector<Intersection> Sphere::local_intersect(Ray r) {
    std::vector<Intersection> intersection_points;
	Intersection root_1(0.0f, this), root_2(0.0f, this);

	Entity sphere_to_ray = Subtract(r.origin, CreatePoint(0.0f, 0.0f, 0.0f));

	float a = DotProduct(r.direction, r.direction);
	float b = 2.0f * DotProduct(r.direction, sphere_to_ray);
	float c = DotProduct(sphere_to_ray, sphere_to_ray) - 1.0f;

	float determinant = b * b - 4 * a * c;

	if (determinant < 0) {
		return intersection_points;
	}

	root_1.t = (-b - sqrt(determinant)) / (2 * a);
	root_2.t = (-b + sqrt(determinant)) / (2 * a);
		
	intersection_points.push_back(root_1);
	intersection_points.push_back(root_2);

    return intersection_points;
}

Entity Sphere::local_normal_at(Entity point, Intersection hit) {
	return CreateVector(point.x, point.y, point.z);
}

std::vector<Intersection> Plane::local_intersect(Ray localRay) {
	std::vector<Intersection> res;

	if (abs(localRay.direction.y) < EPSILON)
		return res;
	
	float t = -localRay.origin.y / localRay.direction.y;
	res.push_back(Intersection(t, this));

	return res;
}

Entity Plane::local_normal_at(Entity point, Intersection hit) {
	return CreateVector(0.0f, 1.0f, 0.0f);
}

std::vector<Intersection> Cube::local_intersect(Ray localRay) {
	auto xaxis = CheckAxis(localRay.origin.x, localRay.direction.x, -1.0f, 1.0f);
	auto yaxis = CheckAxis(localRay.origin.y, localRay.direction.y, -1.0f, 1.0f);
	auto zaxis = CheckAxis(localRay.origin.z, localRay.direction.z, -1.0f, 1.0f);

	float tmin = std::max({ xaxis[0], yaxis[0], zaxis[0] });
	float tmax = std::min({ xaxis[1], yaxis[1], zaxis[1] });

	std::vector<Intersection> res;

	if (tmin > tmax)
		return res;

	res.push_back(Intersection(tmin, this));
	res.push_back(Intersection(tmax, this));

	return res;
}

Entity Cube::local_normal_at(Entity point, Intersection hit) {
	float maxc = std::max({ abs(point.x), abs(point.y), abs(point.z) });

	if (maxc == abs(point.x))
		return CreateVector(point.x, 0.0f, 0.0f);
	else if (maxc == abs(point.y))
		return CreateVector(0.0f, point.y, 0.0f);
	return CreateVector(0.0f, 0.0f, point.z);
}

std::vector<Intersection> Cylinder::local_intersect(Ray localRay) {
	float a = powf(localRay.direction.x, 2) + powf(localRay.direction.z, 2);

	std::vector<Intersection> res;

	// Check if the ray is parallel to the cylinder's side
	if (abs(a) != 0) {
		float b = 2.0f * localRay.origin.x * localRay.direction.x +
			2.0f * localRay.origin.z * localRay.direction.z;
		float c = powf(localRay.origin.x, 2) + powf(localRay.origin.z, 2) - 1.0f;

		float disc = powf(b, 2) - 4 * a * c;

		// No intersections if the discriminant is negative
		if (disc < 0)
			return res;

		float t0 = (-b - sqrtf(disc)) / (2.0f * a);
		float t1 = (-b + sqrtf(disc)) / (2.0f * a);

		// Ensure t0 is the smaller value
		if (t0 > t1)
			std::swap(t0, t1);

		// Check if the intersections lie within the valid height range
		float y0 = localRay.origin.y + t0 * localRay.direction.y;
		if (minimum < y0 && y0 < maximum)
			res.push_back(Intersection(t0, this));

		float y1 = localRay.origin.y + t1 * localRay.direction.y;
		if (minimum < y1 && y1 < maximum)
			res.push_back(Intersection(t1, this));
	}

	// Check for intersections with the cylinder's caps
	IntersectCapsCylinder(*this, localRay, res);

	return res;
}

Entity Cylinder::local_normal_at(Entity point, Intersection hit) {
	float distance = powf(point.x, 2) + powf(point.z, 2);

	if (distance < 1.0f && point.y >= maximum - 0.00001f)
		return CreateVector(0.0f, 1.0f, 0.0f);
	else if (distance < 1.0f && point.y <= minimum + 0.00001f)
		return CreateVector(0.0f, -1.0f, 0.0f);
	return CreateVector(point.x, 0.0f, point.z);
}

std::vector<Intersection> Cone::local_intersect(Ray localRay) {
	std::vector<Intersection> res;

	float a = powf(localRay.direction.x, 2) 
		- powf(localRay.direction.y, 2) 
		+ powf(localRay.direction.z, 2);

	float b = 2 * localRay.origin.x * localRay.direction.x 
		- 2 * localRay.origin.y * localRay.direction.y 
		+ 2 * localRay.origin.z * localRay.direction.z;

	float c = powf(localRay.origin.x, 2)
		- powf(localRay.origin.y, 2)
		+ powf(localRay.origin.z, 2);

	if (!(abs(a - 0) < EPSILON)) {

		float disc = pow(b, 2) - 4 * a * c;

		if (abs(disc - 0) < EPSILON)
			disc = 0;
		// No intersections if the discriminant is negative
		if (disc < 0)
			return res;
		
		float t0 = (-b - sqrtf(disc)) / (2.0f * a);
		float t1 = (-b + sqrtf(disc)) / (2.0f * a);

		// Ensure t0 is the smaller value
		if (t0 > t1)
			std::swap(t0, t1);
		
		// Check if the intersections lie within the valid height range
		float y0 = localRay.origin.y + t0 * localRay.direction.y;
		if (minimum < y0 && y0 < maximum)
			res.push_back(Intersection(t0, this));

		float y1 = localRay.origin.y + t1 * localRay.direction.y;
		if (minimum < y1 && y1 < maximum)
			res.push_back(Intersection(t1, this));
	}
	else if (!(abs(b - 0) < EPSILON)) {
		res = {
			Intersection(-c / (2 * b), this)
		};
	}

	IntersectCapsCone(*this, localRay, res);

	return res;
}

Entity Cone::local_normal_at(Entity point, Intersection hit) {
	float distance = powf(point.x, 2) + powf(point.z, 2);

	if (distance < 1.0f && point.y >= maximum - EPSILON)
		return CreateVector(0.0f, 1.0f, 0.0f);
	else if (distance < 1.0f && point.y <= minimum + EPSILON)
		return CreateVector(0.0f, -1.0f, 0.0f);

	float y = sqrtf(distance);
	if (point.y > 0)
		y = -y;

	return CreateVector(point.x, y, point.z);
}

std::vector<Intersection> Group::local_intersect(Ray localRay) {
	std::vector<Intersection> res;
	
	//IntersectsBox(BoundsOf(*this), localRay) -- boolean
	if (IntersectsBox(BoundsOf(*this), localRay)) {

		for (int i = 0; i < children.size(); i++) {
			//change
			//std::vector<std::vector<float>> temp_transform = children[i]->transform;
			//change
			//children[i]->transform = Matrix4x4_Multiply(transform, children[i]->transform);

			std::vector<Intersection> x = Intersect(*children[i], localRay);
			//change
			//children[i]->transform = temp_transform;

			res.insert(res.end(), x.begin(), x.end());
		}

		std::sort(res.begin(), res.end(), compare_tval);
	}
	return res;
}

Entity Group::local_normal_at(Entity point, Intersection hit) {
	return CreateVector(0.0f, 0.0f, 0.0f);
}

std::vector<Intersection> TestShape::local_intersect(Ray localRay) {
	savedRay = localRay;
	
	std::vector<Intersection> res;
	return res;
}

Entity TestShape::local_normal_at(Entity point, Intersection hit) {
	return CreateVector(point.x, point.y, point.z);
}

Triangle::Triangle(){}

Triangle::Triangle(Entity p1, Entity p2, Entity p3) : p1(p1), p2(p2), p3(p3) {
	e1 = Subtract(p2, p1);
	e2 = Subtract(p3, p1);

	normal = Normalize(CrossProduct(e2, e1));
}

std::vector<Intersection> Triangle::local_intersect(Ray localRay) {
	std::vector<Intersection> res;

	Entity dir_cross_e2 = CrossProduct(localRay.direction, e2);
	float det = DotProduct(e1, dir_cross_e2);

	if (abs(det) < EPSILON)
		return res;

	float f = 1.0f / det;

	Entity p1_to_origin = localRay.origin - p1;
	float u = f * DotProduct(p1_to_origin, dir_cross_e2);
	if (u <= 0 || u >= 1)
		return res;

	Entity origin_cross_e1 = CrossProduct(p1_to_origin, e1);
	float v = f * DotProduct(localRay.direction, origin_cross_e1);
	if (v <= 0 || (u + v) >= 1)
		return res;

	float t = f * DotProduct(e2, origin_cross_e1);

	Intersection i(t, this);
	if (smoothTriangle) {
		i.u = u;
		i.v = v;
	}

	return {i};
}

Entity Triangle::local_normal_at(Entity point, Intersection hit) {
	if (smoothTriangle)
		return n2 * hit.u +
			n3 * hit.v +
			n1 * (1.0f - hit.u - hit.v);

	return normal;
}

CSG::CSG(std::string operation, Shape* left, Shape* right) : operation(operation), left(left), right(right) {
	left->parent = this;
	right->parent = this;
}

Entity CSG::local_normal_at(Entity point, Intersection hit) {
	return CreateVector(0, 0, 0);
}

std::vector<Intersection> CSG::local_intersect(Ray localRay) {
	std::vector<Intersection> res;
	
	if (IntersectsBox(BoundsOf(*this), localRay)) {

		std::vector<Intersection> leftxs = Intersect(*left, localRay);
		std::vector<Intersection> rightxs = Intersect(*right, localRay);

		std::vector<Intersection> xs(leftxs);
		xs.insert(xs.end(), rightxs.begin(), rightxs.end());
		std::sort(xs.begin(), xs.end(), compare_tval);

		return FilterIntersection(this, xs);
	}
	return res;
	
}

















std::vector<Intersection> Intersect(Shape& s, Ray r) {
	Ray localRay = Transform(r, Invert_Matrix(s.transform));

	std::vector<Intersection> res = s.local_intersect(localRay);
	
	return res;
}

std::vector<Intersection> Multiple_Intersections(std::vector<Intersection> list) {
	std::vector<Intersection> series;
	
	for (auto inter : list)
		series.push_back(inter);

	return series;
}

Intersection Hit(const std::vector<Intersection> list) {
	for (const auto& intersection : list) {
		if (intersection.t >= 0) {
			return intersection;
		}
	}

	return Intersection(INFINITY, nullptr);
}


Ray Transform(Ray r, std::vector<std::vector<float>> transformMat) {
	Ray result = Ray();

	result.origin = Matrix_Entity_Multiply(transformMat, r.origin);
	result.direction = Matrix_Entity_Multiply(transformMat, r.direction);

	return result;
}

void Set_Transform(Shape& s, std::vector<std::vector<float>> transformMat) {
	s.transform = transformMat;
}

Color PatternAtShape(Patterns& pattern, Shape& s, Entity Worldpoint) {
	Entity Objectpoint = worldToObject(&s, Worldpoint);
	//Entity Objectpoint = Matrix_Entity_Multiply(Invert_Matrix(s.transform), Worldpoint);
	Entity Patternpoint = Matrix_Entity_Multiply(Invert_Matrix(pattern.transform), Objectpoint);

	return pattern.PatternAt(Patternpoint);
}

std::array<float, 2> CheckAxis(float origin, float direction, float min, float max) {
	float tmin_numerator = (min - origin);
	float tmax_numerator = (max - origin);

	float tmin, tmax;

	if (abs(direction) >= 0.00001) {
		tmin = tmin_numerator / direction;
		tmax = tmax_numerator / direction;
	}
	else {
		tmin = tmin_numerator * INFINITY;
		tmax = tmax_numerator * INFINITY;
	}

	if (tmin > tmax)
		std::swap(tmin, tmax);
	
	return { tmin, tmax };
}

bool CheckCapCylinder(Ray ray, float t) {
	float epsilon = 1e-4f;
	float x = ray.origin.x + t * ray.direction.x;
	float z = ray.origin.z + t * ray.direction.z;

	// Calculate the squared distance from the origin to the intersection point
	float distanceSquared = powf(x, 2) + powf(z, 2);

	return (abs(distanceSquared) <= 1.0f);
}

bool CheckCapCone(Ray ray, float t, float y) {
	float x = ray.origin.x + t * ray.direction.x;
	float z = ray.origin.z + t * ray.direction.z;

	// Calculate the squared distance from the origin to the intersection point
	float distanceSquared = powf(x, 2) + powf(z, 2);

	return (abs(distanceSquared) <= abs(y));
}

void IntersectCapsCylinder(Cylinder& cyl, Ray ray, std::vector<Intersection>& xs) {
	if (cyl.closed == false || abs(ray.direction.y) == 0)
		return;

	float t = (cyl.minimum - ray.origin.y) / ray.direction.y;
	if (CheckCapCylinder(ray, t))
		xs.push_back(Intersection(t, &cyl));

	t = (cyl.maximum - ray.origin.y) / ray.direction.y;
	if (CheckCapCylinder(ray, t))
		xs.push_back(Intersection(t, &cyl));

	return;
}

void IntersectCapsCone(Cone& cone, Ray ray, std::vector<Intersection>& xs) {
	if (cone.closed == false || abs(ray.direction.y - 0) < EPSILON)
		return;

	float t = (cone.minimum - ray.direction.y) / ray.direction.y;
	if (CheckCapCone(ray, t, cone.minimum))
		xs.push_back(Intersection(t, &cone));

	t = (cone.maximum - ray.origin.y) / ray.direction.y;
	if (CheckCapCone(ray, t, cone.maximum))
		xs.push_back(Intersection(t, &cone));
}

Triangle* Smooth_Triangle(Entity p1, Entity p2, Entity p3, Entity n1, Entity n2, Entity n3) {
	Triangle* triangle = new Triangle();
	triangle->p1 = p1;
	triangle->p2 = p2;
	triangle->p3 = p3;
	triangle->n1 = n1;
	triangle->n2 = n2;
	triangle->n3 = n3;

	triangle->e1 = Subtract(p2, p1);
	triangle->e2 = Subtract(p3, p1);

	triangle->smoothTriangle = true;

	return triangle;
}

void AddChild(Group* g, Shape* s) {
	s->parent = g;
	g->children.push_back(s);
}

Entity worldToObject(Shape* s, Entity point) {
	if (s->parent != NULL)
		point = worldToObject(s->parent, point);

	return Matrix_Entity_Multiply(Invert_Matrix(s->transform), point);
}

Entity NormalToWorld(Shape* s, Entity normal) {
	normal = Matrix_Entity_Multiply(Transpose_Matrix(Invert_Matrix(s->transform)), normal);

	normal.w = 0.0f;

	normal = Normalize(normal);

	if (s->parent != NULL)
		normal = NormalToWorld(s->parent, normal);

	return normal;
}

Intersection Intersection_with_uv(float t, Triangle* shape, float u, float v) {
	Intersection i(t, shape);

	i.u = u;
	i.v = v;

	return i;
}

bool IntersectionAllowed(std::string op, bool lhit, bool inl, bool inr) {
	if (op == "union")
		return (lhit && !inr) || (!lhit && !inl);
	else if (op == "intersect")
		return (lhit && inr) || (!lhit && inl);
	else if (op == "difference")
		return (lhit && !inr) || (!lhit && inl);
	return false;
}

std::vector<Intersection> FilterIntersection(CSG* csg, std::vector<Intersection> xs) {
	bool inl = false, inr = false;

	std::vector<Intersection> result;
	for (auto i : xs) {
		bool lhit = csg->left->includes(i.s);

		if (IntersectionAllowed(csg->operation, lhit, inl, inr))
			result.push_back(i);

		if (lhit)
			inl = !inl;
		else
			inr = !inr;
	}

	return result;
}