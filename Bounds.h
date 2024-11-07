#ifndef BOUNDS_H
#define BOUNDS_H

#include "RayCasting.h"

class Bounding_Box {
public:
	Entity minimum = CreatePoint(INFINITY, INFINITY, INFINITY);
	Entity maximum = CreatePoint(-INFINITY, -INFINITY, -INFINITY);

	Bounding_Box();
	Bounding_Box(Entity min, Entity max);
};

void AddPointToBoundingBox(Bounding_Box& box, Entity point);
void AddBoundingBoxToBoundingBox(Bounding_Box& box1, Bounding_Box& box2);
Bounding_Box BoundsOf(TestShape shape);
Bounding_Box BoundsOf(Sphere s);
Bounding_Box BoundsOf(Plane p);
Bounding_Box BoundsOf(Cylinder cyl);
Bounding_Box BoundsOf(Triangle triangle);
Bounding_Box BoundsOf(Group g);
Bounding_Box BoundsOf(CSG csg);
bool Box_Contains_Point(Bounding_Box box, Entity point);
bool Box_Contains_Box(Bounding_Box b1, Bounding_Box b2);
Bounding_Box TranformBoundingBox(Bounding_Box box, std::vector<std::vector<float>> matrix);
Bounding_Box ParentSpaceBoundsOf(TestShape testshape);
Bounding_Box ParentSpaceBoundsOf(Shape& shape);
Bounding_Box ParentSpaceBoundsOf(Sphere shape);
Bounding_Box ParentSpaceBoundsOf(Plane plane);
Bounding_Box ParentSpaceBoundsOf(Cylinder cylinder);
Bounding_Box ParentSpaceBoundsOf(Triangle triangle);
Bounding_Box ParentSpaceBoundsOf(Group group);
Bounding_Box ParentSpaceBoundsOf(CSG csg);
bool IntersectsBox(Bounding_Box box, Ray localRay);

#endif // !BOUNDS_H
