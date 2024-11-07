#ifndef LIGHTINGSHADING_H
#define LIGHTINGSHADING_H

#include "PointVec.h"
#include "RayCasting.h"
#include "Matrices.h"
#include "ColorCanvas.h"


class PointLight {
public:
	Entity position;
	Color intensity;

	PointLight();
	PointLight(Entity pos, Color inten);
};

Entity Reflect(Entity in, Entity normal);
Color Lighting(Material mat, Shape& s, PointLight light, Entity pointPos, Entity eye_v, Entity normal_v, bool in_shadow);
Entity Normal_At(Shape& s, Entity point, Intersection hit);

#endif // !SURFACENORMALS_H

