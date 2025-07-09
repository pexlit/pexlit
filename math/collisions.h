#include "math/rectangle/rectangletn.h"
#include "math/graphics/texture.h"
#include "mattnxn.h"
#include <math/Square.h>
#pragma once
//returns wether a rectangle intersects with another rectangle
//doesn'T collide when the edges touch each other exactly
template<typename T, fsize_t DimensionCount>
bool collides(crectangletn<T, DimensionCount>& r1, crectangletn<T, DimensionCount>& r2)
{
	for (fsize_t dimension = 0; dimension < DimensionCount
		; dimension++) {
		if (r2.pos0[dimension] >= r1.pos0[dimension] + r1.size[dimension] ||
			r1.pos0[dimension] >= r2.pos0[dimension] + r2.size[dimension])
			return false;
	}
	return true;
}

//returns wether a square intersects with another square
//doesn'T collide when the edges touch each other exactly
template<typename T, fsize_t DimensionCount>
bool collides(cSquaretn<T, DimensionCount>& s1, cSquaretn<T, DimensionCount>& s2)
{
	const T& maxDistance = s1.radius + s2.radius;
	for (fsize_t dimension = 0; dimension < DimensionCount
		; dimension++) {
		if (std::abs(s1.center[dimension] - s2.center[dimension]) >= maxDistance)
			return false;
	}
	return true;
}

template<typename T, fsize_t DimensionCount>
bool contains(crectangletn<T, DimensionCount>& rContaining, crectangletn<T, DimensionCount>& rContained)
{
	for (fsize_t dimension = 0; dimension < DimensionCount
		; dimension++) {
		if (rContaining.pos0[dimension] > rContained.pos0[dimension] ||
			rContaining.pos0[dimension] + rContaining.size[dimension] < rContained.pos0[dimension] + rContained.size[dimension])
			return false;
	}
	return true;
}

template<typename T, fsize_t DimensionCount>
bool contains(cSquaretn<T, DimensionCount>& sContaining, cSquaretn<T, DimensionCount>& sContained)
{
	const T& maxDistance = sContaining.radius - sContained.radius;
	//max distance can also be negative, when the containing square is smaller than the contained square. in that case, it'll just fail at the first dimension.
	for (fsize_t dimension = 0; dimension < DimensionCount
		; dimension++) {
		if (std::abs(sContaining.center[dimension] - sContained.center[dimension]) > maxDistance)
			return false;
	}
	return true;
}


bool collides2d(texture* tex1, mat3x3 transform1, texture* tex2, mat3x3 transform2);
bool collides2d(const std::vector<vec2>& v1, const std::vector<vec2>& v2, cbool& checkinside);
bool collides3d(const vec3& p0, const vec3& p1, const vec3& boxp0, const vec3& boxp1);
bool PointInPolygon(cvec2& point, const std::vector<vec2>&  points);
bool collidedistance3d(const vec3& p0, const vec3& p1, const vec3& boxp0, const vec3& boxp1, fp& tmin, fp& tmax);
bool collides2d(cvec2& p0, cvec2& p1, crectangle2& box);
bool collidedistance2d(cvec2& p0, cvec2& p1, crectangle2& box, fp& tmin, fp& tmax);
bool collidedistance2d(cvec2& p0, cvec2& p1, cvec2& boxp0, cvec2& boxp1, fp& tmin, fp& tmax);
bool collidedistance2dDirection(cvec2& p0, cvec2& direction, cvec2& boxp0, cvec2& boxp1, fp& tmin, fp& tmax);
bool collidedistance2dDirection(cvec2& p0, cvec2& direction, crectangle2& box, fp& tmin, fp& tmax);
bool pointleftofline(cvec2& point, cvec2& p0, cvec2& p1);

/// <summary>
/// 
/// </summary>
/// <param name="box0">the moving hitbox</param>
/// <param name="box1">the hitbox which stays still. substract distances to add the distance this hitbox moves</param>
/// <param name="box0MoveDistance">the distance which hitbox 0 will move as a vector</param>
/// <param name="axisCollided">a vector containing a boolean for each axis. 
/// <param name="stuck">if box0 and box1 overlap at the start. this is to distinguish from collisiontime 0
/// when an axis didn't collide, that means that if you'd set the speed of the other axis to 0, the collision would be solved.
/// so both axes only collide when the hitboxes are stuck inside eachother.</param>
/// <returns>a number in range -1 to 1. -1 means that they were inside eachother, 0 means collided instantly, 0.5 means collided after half a timestep, 1 means not collided at all.
/// you should be able to safely multiply speed of the collided axes by this number</returns>
fp collideTime2d(crectangle2& box0, crectangle2& box1, cvec2& box0MoveDistance, vect2<bool>& axisCollided, bool& stuck);
crectangle2 getBoxContaining(crectangle2& box, cvec2& boxSpeed);

bool collides2d(cvec2& a0, cvec2& a1, cvec2& b0, cvec2& b1);
bool collides2d(cvec2& a0, cvec2& a1, cvec2& b0, cvec2& b1, vec2& intersection);


//https://gamedev.stackexchange.com/questions/48587/resolving-a-collision-with-forces
template<fsize_t dimensionCount>
constexpr vecn<dimensionCount> getSpeedAfterCollision(cvect2<vecn<dimensionCount>>& speedsBeforeCollision, cvec2& masses)
{
	if (masses[0] == masses[1])
	{
		return speedsBeforeCollision[1];
	}

	//because an object with infinite mass has an endless amount of energy, sharing is free.
	else if (masses[0] == INFINITY)
	{
		return speedsBeforeCollision[0];
	}
	else if (masses[1] == INFINITY)
	{
		//simplified
		return -speedsBeforeCollision[0] + (2 * speedsBeforeCollision[1]);
	}
	else
	{
		return (speedsBeforeCollision[0] * (masses[0] - masses[1]) + (2 * (masses[1] * speedsBeforeCollision[1]))) / (masses[0] + masses[1]);
	}
}

template<size_t dimensionCount>
constexpr vecn<dimensionCount> getSpeedAfterFriction(cvect2<vecn<dimensionCount>>& speedsBeforeFriction, cvec2& masses)
{
	if (masses[0] == masses[1])
	{
		return (speedsBeforeFriction[0] + speedsBeforeFriction[1]) / 2;
	}

	//because an object with infinite mass has an endless amount of energy, sharing is free.
	else if (masses[0] == INFINITY)
	{
		return speedsBeforeFriction[0];
	}
	else if (masses[1] == INFINITY)
	{
		//simplified
		return speedsBeforeFriction[1];
	}
	else
	{
		//redistribute the energy by mass
		//return the average speed which both particles will get
		return (((speedsBeforeFriction[0] * masses[0]) + (speedsBeforeFriction[1] * masses[1])) / (masses[0] + masses[1]));
	}
}

//the value you can multiply the speed by to get the speed after friction
constexpr fp getFrictionMultiplier(cfp& weight, cfp& frictionWeight) noexcept
{
	//make it so speed * mult * friction = speed

	//newspeed = (speed * weight) / (weight + frictionweight)

	//mult = weight / (weight + frictionweight)

	//is not true
	return weight / (weight + frictionWeight);
}

//multiply this with the terminal velocity to get the adder.
//each step: y = a + xy where:
//y = terminal velocity
//x = terminal velocity multiplier
//a = friction multiplier
//we need to know x
// xy = y - a
// x = (y - a) / y
// x = 1 - a / y
//when y is 1 then x = 1 - a
constexpr fp getTerminalVelocityMultiplier(cfp& frictionMultiplier)
{
	return 1 - frictionMultiplier;
}

//add this to the speed to achieve terminal velocity after infinite iterations.
//each step: y = a - b + xy where:
// y = terminal velocity
// x = terminal velocity multiplier
// a = friction multiplier
// b = substractor
//we need to know x
// xy = y + b - a
// x = (y + b - a) / y
// x = 1 + (b - a) / y
constexpr fp getTerminalVelocityAdder(cfp& frictionMultiplier, cfp& substractor, cfp& terminalVelocity)
{
	return 1 + (substractor - frictionMultiplier) / terminalVelocity;
}

//watch out! velocities being modified
inline void sphereCollisionResponse(cvec2& spherePos1, cfp& sphereMass1, cvec2& spherePos2, cfp& sphereMass2, vec2& sphereVelocity1, vec2& sphereVelocity2)
{
	vec2 U1x, U1y, U2x, U2y, V1x, V1y, V2x, V2y;

	fp m1, m2, x1, x2;
	vec2 v1temp, v1, v2, v1x, v2x, v1y, v2y, x(spherePos1 - spherePos2);

	x.normalize();
	v1 = sphereVelocity1;
	x1 = vec2::dot(x, v1);
	v1x = x * x1;
	v1y = v1 - v1x;
	m1 = sphereMass1;

	x = x * -1;
	v2 = sphereVelocity2;
	x2 = vec2::dot(x, v2);
	v2x = x * x2;
	v2y = v2 - v2x;
	m2 = sphereMass2;

	cfp frictionMult = 1;// 0.9;
	vec2 withoutFriction1 = vec2(v1x * (m1 - m2) / (m1 + m2) + v2x * (2 * m2) / (m1 + m2) + v1y);
	sphereVelocity2 = vec2(v1x * (2 * m1) / (m1 + m2) + v2x * (m2 - m1) / (m1 + m2) + v2y);
	sphereVelocity1 = math::lerp(sphereVelocity2, withoutFriction1, frictionMult);
	sphereVelocity2 = math::lerp(withoutFriction1, sphereVelocity2, frictionMult);
}