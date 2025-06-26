#include "math/vector/vectn.h"
#pragma once
struct Sphere
{
	vec3 center;
	fp radius;
	Sphere(cvec3 &center, cfp &radius) : center(center), radius(radius)
	{
	}
	Sphere() = default;
};