#ifndef PATTERNS_H
#define PATTERNS_H

#include <cmath>

#include "ColorCanvas.h"
#include "PointVec.h"
#include "Matrices.h"

enum RING_AXES {
	XY_AXES_RING = 1,
	YZ_AXES_RING = 2,
	XZ_AXES_RING = 3
};

class Patterns {
public:
	std::vector<std::vector<float>> transform = {
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};

	virtual Color PatternAt(Entity point) = 0;
};

class StripePattern : public Patterns{
public:
	Color a, b;

	StripePattern(Color a, Color b);
	Color PatternAt(Entity point);
};

class Gradient : public Patterns {
public:
	Color a, b;

	Gradient(Color a, Color b);
	Color PatternAt(Entity point);
};

class RingPattern : public Patterns {
public:
	Color a, b;
	int axes;

	RingPattern(Color a, Color b, int axes);
	Color PatternAt(Entity point);
};

class CheckeredPattern : public Patterns {
public:
	Color a, b;

	CheckeredPattern(Color a, Color b);
	Color PatternAt(Entity point);
};

class RadialGradient : public Patterns {
public:
	Gradient grad;
	RingPattern ringPat;

	RadialGradient(Gradient grad, RingPattern ringPat);
	Color PatternAt(Entity point);
};

class CheckeredStrips : public Patterns {
public:
	StripePattern Stripes1, Stripes2;

	CheckeredStrips(StripePattern s1, StripePattern s2);
	Color PatternAt(Entity point);
};

class Test_Pattern : public Patterns {
public:
	Test_Pattern();
	Color PatternAt(Entity point);
};

void SetPatternTransform(Patterns& pattern, std::vector<std::vector<float>> transform);

#endif // !PATTERNS_H
