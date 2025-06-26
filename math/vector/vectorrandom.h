#pragma once
#include "math/random/random.h"
#include "math/rectangle/rectangletn.h"



template<fsize_t dimensionCount = 3>
inline vecn< dimensionCount> randomPointInUnitSphere(std::mt19937& generator, cfp& desiredRadius = 1) {
	//https://math.stackexchange.com/questions/87230/picking-random-points-in-the-volume-of-sphere-with-uniform-probability
	std::uniform_real_distribution<fp>  uni(0.0, 1.0);
	//normal distribution with mean of 0 and variance of 1 (so between 0, 1)
	std::normal_distribution<fp>        norm(0.0, 1.0);

	// 1) draw radius
	fp u = uni(generator);
	fp r = std::cbrt(u);

	// 2) draw 3 Gaussians

	vecn<dimensionCount> normalDistributedNumber = {};
	for (size_t i = 0; i < dimensionCount; i++) {
		normalDistributedNumber[i] = norm(generator);
	};

	// 3) normalize to unit vector and scale by r
	return normalDistributedNumber * ((r * desiredRadius) / normalDistributedNumber.length());
}

inline vec2 randomPointInUnitCircle(std::mt19937& generator) {
	return vec2::getrotatedvector(randFp(generator, math::PI2)) * sqrt(randFp(generator, (fp)0.5));
}

inline vec2 getRandomPointOnEllipse(std::mt19937& generator, crectangle2& ellipseRect)
{
	//https://stackoverflow.com/questions/5837572/generate-a-random-point-within-a-circle-uniformly
	return ellipseRect.pos0 + (0.5 + randomPointInUnitCircle(generator) * 0.5) * ellipseRect.size;
}

inline vec3 getRandomPointInSphere(std::mt19937& generator, crectangle3& ellipseRect)
{
	//https://stackoverflow.com/questions/5837572/generate-a-random-point-within-a-circle-uniformly
	//cbrt(rand * 0.125) to eliminate 
	return ellipseRect.pos0 + (0.5 + randomPointInUnitSphere(generator) * 0.5) * ellipseRect.size;
}

inline vec2 getRandomVector(std::mt19937& generator, fp maxLength)
{
	return vec2::getrotatedvector(randFp(generator, math::PI2)) * randFp(generator, maxLength);
}