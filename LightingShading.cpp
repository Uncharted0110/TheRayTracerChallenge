#include "LightingShading.h"

#define BLACK Color(0.0f, 0.0f, 0.0f)

PointLight::PointLight() {}

PointLight::PointLight(Entity pos, Color inten) : position(pos), intensity(inten) {}

Entity Normal_At(Shape& s, Entity point, Intersection hit) {
	//std::vector<std::vector<float>> inv = Invert_Matrix(s.transform);
	Entity localPoint = worldToObject(&s, point);
	
	Entity localNormal = s.local_normal_at(localPoint, hit);

	return NormalToWorld(&s, localNormal);
	/*
	std::vector<std::vector<float>> transInv = Transpose_Matrix(inv);
	Entity worldNormal = Matrix_Entity_Multiply(transInv, localNormal);

	worldNormal.w = 0.0f;

	return Normalize(worldNormal);
	*/
}

Entity Reflect(Entity in, Entity normal) {
	return Subtract(in, Multiply(Multiply(normal, DotProduct(in, normal)), 2.0f)); // R = I - 2 * (I.N) * N
}

Color Lighting(Material mat, Shape& s, PointLight light, Entity pointPos, Entity eye_v, Entity normal_v, bool in_shadow) {
	Color color;
	
	if (mat.pattern != NULL)
		color = PatternAtShape(*mat.pattern, s, pointPos);
	else
		color = mat.color;

	Color ambient, diffuse, specular;
	
	// Combine surface color with the light's intensity
	Color effectiveColor = color * light.intensity;

	// Find direction to the light source
	Entity light_v = Normalize(Subtract(light.position, pointPos));

	// Compute ambient contribution
	ambient = effectiveColor * mat.ambient;

	if (in_shadow)
		return ambient;

	// lightv_dot_normalv is dot product of light vector and normal vector
	float lightv_dot_normalv = DotProduct(light_v, normal_v);
	
	if (lightv_dot_normalv < 0) { // light is on the other side of surface
		diffuse = BLACK;
		specular = BLACK;
	}
	else { 
		// Compute diffuse contribution
		diffuse = effectiveColor * mat.diffuse * lightv_dot_normalv;

		Entity reflect_v = Reflect(Negate(light_v), normal_v);
		float reflectv_dot_eyev = DotProduct(reflect_v, eye_v);
		
		if (reflectv_dot_eyev <= 0)
			specular = BLACK;
		else {
			float factor = pow(reflectv_dot_eyev, mat.shininess);
			specular = light.intensity * mat.specular * factor;
		}
	}

	return ambient + diffuse + specular;
}