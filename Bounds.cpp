#include "Bounds.h"

#include <array>

Bounding_Box::Bounding_Box() {};

Bounding_Box::Bounding_Box(Entity min, Entity max) : minimum(min), maximum(max) {};

void AddPointToBoundingBox(Bounding_Box& box, Entity point) {
	box.minimum.x = std::min(box.minimum.x, point.x);
	box.minimum.y = std::min(box.minimum.y, point.y);
	box.minimum.z = std::min(box.minimum.z, point.z);

	box.maximum.x = std::max(box.maximum.x, point.x);
	box.maximum.y = std::max(box.maximum.y, point.y);
	box.maximum.z = std::max(box.maximum.z, point.z);
}

void AddBoundingBoxToBoundingBox(Bounding_Box& box1, Bounding_Box& box2) {
	AddPointToBoundingBox(box1, box2.minimum);
	AddPointToBoundingBox(box1, box2.maximum);
}

Bounding_Box BoundsOf(TestShape shape) {
	return Bounding_Box(CreatePoint(-1.0f, -1.0f, -1.0f), CreatePoint(1.0f, 1.0f, 1.0f));
}

Bounding_Box BoundsOf(Sphere s) {
	return Bounding_Box(CreatePoint(-1.0f, -1.0f, -1.0f), CreatePoint(1.0f, 1.0f, 1.0f));
}

Bounding_Box BoundsOf(Plane p) {
	return Bounding_Box(CreatePoint(-INFINITY, 0.0f, INFINITY), CreatePoint(INFINITY, 0.0f, INFINITY));
}

Bounding_Box BoundsOf(Cylinder cyl) {
	return Bounding_Box(CreatePoint(-1.0f, cyl.minimum, -1.0f), CreatePoint(1.0f, cyl.maximum, 1.0f));
}

Bounding_Box BoundsOf(Triangle triangle) {
	Bounding_Box box = Bounding_Box();

	AddPointToBoundingBox(box, triangle.p1);
	AddPointToBoundingBox(box, triangle.p2);
	AddPointToBoundingBox(box, triangle.p3);

	return box;
}

Bounding_Box BoundsOf(Group g) {
	Bounding_Box box = Bounding_Box();

	for (int i = 0; i < g.children.size(); i++) {
		Bounding_Box cbox = ParentSpaceBoundsOf(*g.children[i]);
		AddBoundingBoxToBoundingBox(box, cbox);
	}

	return box;
}

Bounding_Box BoundsOf(CSG csg) {
	Bounding_Box box = Bounding_Box();

	Bounding_Box lbox = ParentSpaceBoundsOf(*csg.left);
	AddBoundingBoxToBoundingBox(box, lbox);

	Bounding_Box rbox = ParentSpaceBoundsOf(*csg.right);
	AddBoundingBoxToBoundingBox(box, rbox);

	return box;
}

bool Box_Contains_Point(Bounding_Box box, Entity point) {
	if (box.minimum.x <= point.x && box.maximum.x >= point.x)
		if (box.minimum.y <= point.y && box.maximum.y >= point.y)
			if (box.minimum.z <= point.z && box.maximum.z >= point.z)
				return true;
	return false;
}

bool Box_Contains_Box(Bounding_Box b1, Bounding_Box b2) {
	return (Box_Contains_Point(b1, b2.minimum) && Box_Contains_Point(b1, b2.maximum));
}

Bounding_Box TranformBoundingBox(Bounding_Box box, std::vector<std::vector<float>> matrix) {
	Entity p1 = box.minimum;
	Entity p2 = CreatePoint(box.minimum.x, box.minimum.y, box.maximum.z);
	Entity p3 = CreatePoint(box.minimum.x, box.maximum.y, box.minimum.z);
	Entity p4 = CreatePoint(box.minimum.x, box.maximum.y, box.maximum.z);
	Entity p5 = CreatePoint(box.maximum.x, box.minimum.y, box.minimum.z);
	Entity p6 = CreatePoint(box.maximum.x, box.minimum.y, box.maximum.z);
	Entity p7 = CreatePoint(box.maximum.x, box.maximum.y, box.minimum.z);
	Entity p8 = box.maximum;

	Bounding_Box newBox = Bounding_Box();

	for (Entity p : {p1, p2, p3, p4, p5, p6, p7, p8}) {
		AddPointToBoundingBox(newBox, Matrix_Entity_Multiply(matrix, p));
	}

	return newBox;
}

Bounding_Box ParentSpaceBoundsOf(Shape& shape) {
	if (Sphere* sphere = dynamic_cast<Sphere*>(&shape)) {
		// Compute and return the bounding box for the sphere
		return ParentSpaceBoundsOf(*sphere);
	}
	else if (Plane* plane = dynamic_cast<Plane*>(&shape)) {
		return ParentSpaceBoundsOf(*plane);
	}
	else if (Cylinder* cylinder = dynamic_cast<Cylinder*>(&shape)) {
		// Compute and return the bounding box for the cylinder
		return ParentSpaceBoundsOf(*cylinder);
	}
	else if (TestShape* testshape = dynamic_cast<TestShape*>(&shape)) {
		return ParentSpaceBoundsOf(*testshape);
	}
	else if (Group* group = dynamic_cast<Group*>(&shape)) {
		return ParentSpaceBoundsOf(*group);
	}
	else if (Triangle* triangle = dynamic_cast<Triangle*>(&shape)) {
		return ParentSpaceBoundsOf(*triangle);
	}
	else if (CSG* csg = dynamic_cast<CSG*>(&shape)) {
		return ParentSpaceBoundsOf(*csg);
	}
	else {
		std::cout << "Bounding of this shape is not made" << std::endl;
		return Bounding_Box();
	}
}

Bounding_Box ParentSpaceBoundsOf(Sphere sphere) {
	//Transform the bounding box of the particular shape(here sphere) to the transformation of the shape
	return TranformBoundingBox(BoundsOf(sphere), sphere.transform);
}

Bounding_Box ParentSpaceBoundsOf(Plane plane) {
	return TranformBoundingBox(BoundsOf(plane), plane.transform);
}

Bounding_Box ParentSpaceBoundsOf(Cylinder cylinder) {
	return TranformBoundingBox(BoundsOf(cylinder), cylinder.transform);
}

Bounding_Box ParentSpaceBoundsOf(TestShape testshape) {
	return TranformBoundingBox(BoundsOf(testshape), testshape.transform);
}

Bounding_Box ParentSpaceBoundsOf(Triangle triangle) {
	return TranformBoundingBox(BoundsOf(triangle), triangle.transform);
}

Bounding_Box ParentSpaceBoundsOf(Group group) {
	return TranformBoundingBox(BoundsOf(group), group.transform);
}

Bounding_Box ParentSpaceBoundsOf(CSG csg) {
	return TranformBoundingBox(BoundsOf(csg), csg.transform);
}

bool IntersectsBox(Bounding_Box box, Ray localRay) {
	auto xaxis = CheckAxis(localRay.origin.x, localRay.direction.x, box.minimum.x, box.maximum.x);
	auto yaxis = CheckAxis(localRay.origin.y, localRay.direction.y, box.minimum.y, box.maximum.y);
	auto zaxis = CheckAxis(localRay.origin.z, localRay.direction.z, box.minimum.z, box.maximum.z);

	float tmin = std::max({ xaxis[0], yaxis[0], zaxis[0] });
	float tmax = std::min({ xaxis[1], yaxis[1], zaxis[1] });

	if (tmin > tmax)
		return false;
	
	return true;
}