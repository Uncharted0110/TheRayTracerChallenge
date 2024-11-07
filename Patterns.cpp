#include "Patterns.h"
#include "RayCasting.h"

class Shape;

StripePattern::StripePattern(Color a, Color b) : a(a), b(b) {}

Color StripePattern::PatternAt(Entity point) {
	float x_coord = floor(point.x);

	if (int(x_coord) % 2 == 0)
		return a;
	else
		return b;
}

Gradient::Gradient(Color a, Color b) : a(a), b(b) {}

Color Gradient::PatternAt(Entity point) {
	return a + (b - a) * (point.x - floor(point.x));
}

RingPattern::RingPattern(Color a, Color b, int axes) : a(a), b(b) ,axes(axes) {}

Color RingPattern::PatternAt(Entity point) {
	float exp = 0.0f;

	switch (axes)
	{
	case 1:
		exp = (pow(point.x, 2.0f) + pow(point.y, 2.0f));
		break;
	case 2:
		exp = (pow(point.y, 2.0f) + pow(point.z, 2.0f));
		break;
	case 3:
		exp = (pow(point.z, 2.0f) + pow(point.x, 2.0f));
		break;
	default:
		std::cout << "Invalid axes" << std::endl;
		break;
	}

	exp = sqrtf(exp);

	if (int(floor(exp)) % 2 == 0)
		return a;
	else
		return b;
}

CheckeredPattern::CheckeredPattern(Color a, Color b) : a(a), b(b) {}

Color CheckeredPattern::PatternAt(Entity point) {
	float exp = floor(point.x) + floor(point.y) + floor(point.z);

	if (int(exp) % 2 == 0)
		return a;
	else
		return b;
}

RadialGradient::RadialGradient(Gradient grad, RingPattern ringPat) : grad(grad), ringPat(ringPat) {}

Color RadialGradient::PatternAt(Entity point) {
	float exp = 0.0f;

	switch (ringPat.axes)
	{
	case 1:
		exp = (pow(point.x, 2.0f) + pow(point.y, 2.0f));
		break;
	case 2:
		exp = (pow(point.y, 2.0f) + pow(point.z, 2.0f));
		break;
	case 3:
		exp = (pow(point.z, 2.0f) + pow(point.x, 2.0f));
		break;
	default:
		std::cout << "Invalid axes" << std::endl;
		break;
	}

	exp = sqrtf(exp);

	return grad.a + (grad.b - grad.a) * (exp - floor(exp));
}

CheckeredStrips::CheckeredStrips(StripePattern s1, StripePattern s2) : Stripes1(s1), Stripes2(s2) {}

Color CheckeredStrips::PatternAt(Entity point) {
	float exp = floor(point.x) + floor(point.y) + floor(point.z);
	point = Matrix_Entity_Multiply(transform, point);

	if (int(exp) % 2 == 0) {
		point = Matrix_Entity_Multiply(Invert_Matrix(Stripes1.transform), point);
		return Stripes1.PatternAt(point);
	}
	else {
		point = Matrix_Entity_Multiply(Invert_Matrix(Stripes2.transform), point);
		return Stripes2.PatternAt(point);
	}
}

Test_Pattern::Test_Pattern() {}

Color Test_Pattern::PatternAt(Entity point) {
	return Color(point.x, point.y, point.z);
}

void SetPatternTransform(Patterns& pattern, std::vector<std::vector<float>> transformMat) {
	pattern.transform = transformMat;
}