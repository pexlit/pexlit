#pragma once
#include "math/random/random.h"
#include "math/rectangle/rectangletn.h"

inline vec2 getRandomPointOnEllipse(std::mt19937& generator, crectangle2& ellipseRect)
{
	//https://stackoverflow.com/questions/5837572/generate-a-random-point-within-a-circle-uniformly
	return ellipseRect.pos0 + (vec2(0.5) + vec2::getrotatedvector(randFp(generator, math::PI2)) * sqrt(randFp(generator, (fp)1))) * ellipseRect.size;
}

//todo: optimize
inline vec3 getRandomPointInSphere(std::mt19937& generator, crectangle3& ellipseRect)
{
	//https://stackoverflow.com/questions/5837572/generate-a-random-point-within-a-circle-uniformly
	return ellipseRect.pos0 + (vec3(0.5) + vec3::getrotatedvector(randFp(generator, math::PI2), randFp(generator, math::PI * -0.5, math::PI * 0.5) * sqrt(randFp(generator, (fp)1))) * std::cbrt(randFp(generator, (fp)1))) * ellipseRect.size;
}

inline vec2 getRandomVector(std::mt19937& generator, fp maxLength)
{
	return vec2::getrotatedvector(randFp(generator, math::PI2)) * randFp(generator, maxLength);
}