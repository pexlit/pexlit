#pragma once
#include "math/sphere/sphere.h"
//https://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection
//center = vec3(0,0,0)
//ray direction doesn't have to be normalized!
//t0 = in, t1 = out
inline bool collideraysphere(const vec3& rayorigin, const vec3& raydirection, cfp& radius, fp& t0, fp& t1) {
	cfp& a = raydirection.lengthSquared();
	cfp& b = 2.0 * vec3::dot(rayorigin, raydirection);
	cfp& c = vec3::dot(rayorigin, rayorigin) - radius * radius;
	cfp& discriminant = b * b - 4 * a * c;
	if (discriminant < 0) {
		return false;
	}
	else
	{
		cfp& division2a = (2.0 * a);
		cfp& middle = -b / division2a;
		cfp& radiusatintersection = sqrt(discriminant) / division2a;
		t0 = middle - radiusatintersection;
		t1 = middle + radiusatintersection;
		return true;
	}
}
inline bool collideraysphere(const vec3& rayorigin, const vec3& raydirection, const Sphere& sphere, fp& t0, fp& t1)
{
	return collideraysphere(rayorigin - sphere.center, raydirection, sphere.radius, t0, t1);
}

template<fsize_t dimensionCount>
//https://studiofreya.com/3d-math-and-physics/little-more-advanced-collision-detection-spheres/
inline bool collideSpheresSpeed(cvecn<dimensionCount>& spherePos1, cfp& sphereRadius1, cvecn<dimensionCount>& sphereVelocity1, cvecn<dimensionCount>& spherePos2, cfp& sphereRadius2, cvecn<dimensionCount>& sphereVelocity2, fp& timePoint)
{

	vecn<dimensionCount> s = spherePos1 - spherePos2; // vector between the centers of each sphere
	vecn<dimensionCount> v = sphereVelocity1 - sphereVelocity2; // relative velocity between spheres
	fp r = sphereRadius1 + sphereRadius2;

	fp c = s.lengthSquared() - r * r; // if negative, they overlap
	if (c < 0.0) // if true, they already overlap
	{
		timePoint = .0;
		return true;
	}

	fp a = v.lengthSquared();

	fp b = vecn<dimensionCount>::dot(v, s);
	if (b >= 0.0)
		return false; // does not move towards each other

	fp d = b * b - a * c;
	if (d < 0.0)
		return false; // no real roots ... no collision

	timePoint = (-b - sqrt(d)) / a;

	return true;

}