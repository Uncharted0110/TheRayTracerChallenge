#include "Scene.h"
#include <algorithm>
#include <iomanip>

#define Intersections std::vector<Intersection>
#define BLACK Color(0.0f, 0.0f, 0.0f);
#define EPSILON 0.01f

World::World() {}

World DeafultWorld() {
	PointLight light(CreatePoint(-10.0f, 10.0f, -10.0f), Color(1.0f, 1.0f, 1.0f));

	Sphere* s1 = new Sphere();
	s1->mat.color = Color(0.8f, 1.0f, 0.6f);
	s1->mat.diffuse = 0.7f;
	s1->mat.specular = 0.2f;

	Sphere* s2 = new Sphere();
	s2->transform = Scaling(0.5f, 0.5f, 0.5f);

	World w = World();

	w.light = light;
	w.object_list = { s1, s2 };

	return w;
}

Intersections IntersectWorld(World& w, Ray r) {
	Intersections xs;

	for (int i = 0; i < w.object_list.size(); i++) {
		Intersections x = Intersect(*w.object_list[i], r);
		
		xs.insert(xs.end(), x.begin(), x.end());
	}

	std::sort(xs.begin(), xs.end(), compare_tval);

	return xs;
}

Computation::Computation(Intersection i, Ray r, std::vector<Intersection> list) : t(i.t), object(*i.s) {
	point = Position_At_t(r, t);
	eye_v = Negate(r.direction);
	normal_v = Normal_At(object, point, i);

	if (DotProduct(normal_v, eye_v) < 0) {
		inside = true;
		normal_v = Negate(normal_v);
	}
	else
		inside = false;

	reflectv = Reflect(r.direction, normal_v);

	over_point = Add(point, Multiply(normal_v, EPSILON));
	under_point = Subtract(point, Multiply(normal_v, EPSILON));

	std::vector<Shape*> containers = {};

	for (Intersection a : list) {

		bool hit = a.t == i.t && a.s == i.s;

		if (hit) {
			if (containers.size() == 0)
				n1 = 1.0f;
			else
				n1 = containers[containers.size() - 1]->mat.refractive_index;
		}

		auto iter = std::find(containers.begin(), containers.end(), a.s);

		if (iter != containers.end())
			containers.erase(iter);
		else
			containers.push_back(a.s);

		if (hit) {
			if (containers.size() == 0)
				n2 = 1.0f;
			else
				n2 = containers[containers.size() - 1]->mat.refractive_index;
			break;
		}
	}
}

bool IsShadowed(World w, Entity point) {
	// Vector from point to light source
	Entity v = Subtract(w.light.position, point);

	// Magnitude of that vector and it's direction
	float distance = Magnitude(v);
	Entity direction = Normalize(v);

	// Create a ray from the point to the light source
	Ray r(point, direction);

	// Get list of intersections in the path of the ray
	std::vector<Intersection> intersections = IntersectWorld(w, r);
	
	Intersection h = Hit(intersections);
	if (h.t != INFINITY && h.t < distance)
		return true;
	else
		return false;

	return false;
}

Color ShadeHit(World w, Computation comps, int remaining) {
	bool shadowed = IsShadowed(w, comps.over_point);

	Color surface = Lighting(comps.object.mat, comps.object,
		w.light,
		comps.over_point, comps.eye_v, comps.normal_v,
		shadowed);

	Color reflected = ReflectedColor(w, comps, remaining);
	Color refracted = RefractedColor(w, comps, remaining);

	Material material = comps.object.mat;
	if (material.reflective > 0 && material.transparency > 0) {
		float reflectance = Schlick(comps);
		return surface + reflected * reflectance + refracted * (1.0f - reflectance);
	}

	return surface + reflected + refracted;
}

Color Color_At(World w, Ray r, int remaining) {
	Intersections xs = IntersectWorld(w, r);
	
	Color c = BLACK;

	Intersection x = Hit({ xs });

	if (x.t != INFINITY) {

		Computation comps(x, r, xs);

		c = ShadeHit(w, comps, remaining);
	}

	return c;
}

std::vector<std::vector<float>> View_Transform(Entity from, Entity to, Entity up) {
	Entity forward_v = Normalize(Subtract(to, from));

	Entity up_n = Normalize(up);

	Entity left_v = CrossProduct(forward_v, up_n);

	Entity true_up = CrossProduct(left_v, forward_v);

	std::vector<std::vector<float>> orientation = {
		{left_v.x, left_v.y, left_v.z, 0.0f},
		{true_up.x, true_up.y, true_up.z , 0.0f},
		{-forward_v.x, -forward_v.y, -forward_v.z, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};

	std::vector<std::vector<float>> T = Translation(-from.x, -from.y, -from.z);

	return Matrix4x4_Multiply(orientation, T);
}

Camera::Camera(int horizontal, int vertical, float fov) : hsize(horizontal), vsize(vertical), field_of_view(fov) {
	transform = {
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};

	float half_view = tanf(fov / 2.0f);

	float aspect = float(hsize) / float(vsize);

	if (aspect >= 1) {
		half_width = half_view;
		half_height = half_view / aspect;
	}
	else {
		half_height = half_view;
		half_width = half_view * aspect;
	}

	pixelSize = (half_height * 2.0f) / vsize;
}

Ray RayForPixel(Camera c, int pixel_x, int pixel_y) {
	// the offset from the edge of the canvas to the pixel's center... this is in order to get the ray to go through the center of the pixel
	float xoffset = (pixel_x + 0.5f) * c.pixelSize;
	float yoffset = (pixel_y + 0.5f) * c.pixelSize;

	// the world coordinates:
	// the camera is at center and looking towards -ve z axis, so +ve x is towards left
	// the half_width tells how much left or right the camera can see at 1 unit in front of it
	// the half_height tells how much up or down the camera can see at 1 unit in front of it
	float world_x = c.half_width - xoffset;
	float world_y = c.half_height - yoffset;

	// using camera's transformation matrix, transform the pixelPoint and the origin, and get ray's direction vector
	std::vector<std::vector<float>> inverse = Invert_Matrix(c.transform);
	Entity pixelPoint = Matrix_Entity_Multiply(inverse, CreatePoint(world_x, world_y, -1.0f));
	Entity origin = Matrix_Entity_Multiply(inverse, CreatePoint(0.0f, 0.0f, 0.0f));
	Entity direction = Normalize(Subtract(pixelPoint, origin));

	return Ray(origin, direction);
}

void LoadingBar(float percentage) {
	const int barWidth = 50;  // Width of the loading bar

	//hide the cursor
	std::cout << "\033[?25l";

	// Calculate the number of characters that should be filled
	int progress = static_cast<int>((percentage * barWidth) / 100.0f);

	std::cout << "[";
	for (int i = 0; i < barWidth; i++) {
		if (i < progress) {
			std::cout << "=";  // Filled part of the bar
		}
		else {
			std::cout << " ";  // Empty part of the bar
		}
	}
	std::cout << "] " << std::fixed << std::setprecision(2) << percentage << "%\r";  // Display the percentage at the end of the bar
	std::cout.flush();

	// Show the cursor when complete
	if (percentage >= 100.0f) {
		std::cout << "\033[?25h\n";
	}
}


Canvas Render(Camera c, World w) {
	Canvas canvas(c.hsize, c.vsize);
	float i = 0.0f;
	for (int y = 0; y < c.vsize; y++) {
		for (int x = 0; x < c.hsize; x++) {
			i++;
			float percentage = (i / (c.hsize * c.vsize)) * 100.0f;
			LoadingBar(percentage);
			Ray r = RayForPixel(c, x, y);
			Color color = Color_At(w, r);
			canvas.Write_Pixel(x, y, color);
		}
	}

	return canvas;
}

Color ReflectedColor(World w, Computation comps, int remaining) {
	if (remaining <= 0)
		return Color(0.0f, 0.0f, 0.0f);
	
	if (comps.object.mat.reflective == 0)
		return Color(0.0f, 0.0f, 0.0f);
	
	Ray reflected_ray = Ray(comps.over_point, comps.reflectv);
	Color c = Color_At(w, reflected_ray, remaining - 1);

	return c * comps.object.mat.reflective;
}

Sphere* GlassSphere() {
	Sphere* s = new Sphere();
	s->mat.transparency = 1.0f;
	s->mat.refractive_index = 1.5f;

	return s;
}

Color RefractedColor(World w, Computation comps, int remaining) {
	if (remaining <= 0)
		return Color(0.0f, 0.0f, 0.0f);

	if (comps.object.mat.transparency == 0)
		return Color(0.0f, 0.0f, 0.0f);

	float n_ratio = comps.n1 / comps.n2;

	float cos_i = DotProduct(comps.eye_v, comps.normal_v);

	float sin2_t = powf(n_ratio, 2) *(1.0f - powf(cos_i, 2));

	if (sin2_t > 1)
		return Color(0.0f, 0.0f, 0.0f);

	float cos_t = sqrtf(1.0f - sin2_t);

	Entity direction = comps.normal_v * (n_ratio * cos_i - cos_t) - comps.eye_v * n_ratio;

	Ray refracted_ray(comps.under_point, direction);

	Color c = Color_At(w, refracted_ray, remaining - 1) * comps.object.mat.transparency;

	return c;
}

float Schlick(Computation comps) {
	float cos = DotProduct(comps.eye_v, comps.normal_v);

	if (comps.n1 > comps.n2) {
		float n_ratio = comps.n1 / comps.n2;
		
		float sin2_t = powf(n_ratio, 2.0f) * (1.0f - powf(cos, 2.0f));
		
		if (sin2_t > 1.0f)
			return 1.0f;

		float cos_t = sqrtf(1.0f - sin2_t);

		cos = cos_t;
	}

	float r0 = powf(((comps.n1 - comps.n2) / (comps.n1 + comps.n2)), 2);

	return r0 + (1.0f - r0) * powf((1.0f - cos), 5);
}