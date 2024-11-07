#ifndef SCENE_H
#define SCENE_H

#include "LightingShading.h"

class World {
public:
	PointLight light;
	std::vector<Shape*> object_list;

	World();
};

class Computation {
public:
	float t, n1, n2;
	Entity point, eye_v, normal_v, over_point, reflectv, under_point;
	Shape& object;
	bool inside;

	Computation(Intersection i, Ray r, std::vector<Intersection> list);
};

class Camera {
public:
	int hsize, vsize;
	float field_of_view, half_width, half_height;
	std::vector<std::vector<float>> transform;
	float pixelSize;

	Camera(int horizontal, int vertical, float fov);
};

World DeafultWorld();
Color Color_At(World w, Ray r, int remaining = 6);
std::vector<std::vector<float>> View_Transform(Entity from, Entity to, Entity up);
Ray RayForPixel(Camera c, int x, int y);
Canvas Render(Camera c, World w);
bool IsShadowed(World w, Entity point);
Color ShadeHit(World w, Computation comps, int remaining);
Color ReflectedColor(World w, Computation comps, int remaining);
Sphere* GlassSphere();
Color RefractedColor(World w, Computation comps, int remaining);
void LoadingBar(float percentage);
float Schlick(Computation comps);

#endif // !SCENE_H
