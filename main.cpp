#include <iostream>
#include <vector>
#include <math.h>

#include "PointVec.h"
#include "ColorCanvas.h"
#include "Matrices.h"
#include "RayCasting.h"
#include "LightingShading.h"
#include "Scene.h"
#include "Bounds.h"
#include "OBJFiles.h"

#define PI 3.14159f
#define SCALING_FACTOR 3 / 8
#define Matrix std::vector<std::vector<float>>
#define IntersectionPoints std::vector<Intersection>
#define RED Color(1.0f, 0.0f, 0.0f)
#define BLACK Color(0.1f, 0.1f, 0.1f)
#define WHITE Color(.75f, .75f, .75f)

int main(void) {
	Sphere* s1 = new Sphere();
	s1->ID = "Sphere";
	s1->mat.color = Color(1.0f, 0.690f, 0.486f);
	Sphere* s2 = new Sphere();
	s2->ID = "Sphere";
	s2->transform = Translation(0, 0, 0.7);
	s2->mat.color = Color(1.0f, 0.690f, 0.486f);

	CSG* c = new CSG("union", s1, s2);

	Plane* floor = new Plane();
	floor->ID = "Floor";
	floor->mat.pattern = new CheckeredPattern(WHITE, BLACK);
	floor->transform = Translation(0, -1, 0);

	World w = World();
	w.light = PointLight(CreatePoint(10, 3, 4), Color(0.9f, 0.9f, 0.9f));
	w.object_list = { c, floor };

	Camera cam(512, 512, 0.45f);
	cam.transform = View_Transform(
		CreatePoint(7, 3, 4),
		CreatePoint(0, 0, 0),
		CreateVector(0, 1, 0)
	);

	Canvas canvas = Render(cam, w);

	PPM ppm(canvas, "CSG.ppm");
}
