#pragma once
#include <math/rectangle/rectangletn.h>
#include <math/physics/gravity/gravity.h>
#include <math/physics/RigidBody.h>
#include <deque>
#include <math/Square.h>
//a deque is an array which can resize without moving or calling constructors on previous objects
extern std::deque<struct BarnesHutTile> storage;
int storageIndex = 0;
constexpr fp smallestTileSize = 1;
/// <summary>
/// this gravity simulator makes use of the barnes-hut gravity algorithm
/// http://arborjs.org/docs/barnes-hut
/// </summary>
struct BarnesHutTile
{
	//we store the body, as it's also used in collision detection
	std::vector<const RigidBody*> bodies;
	vec3 centerOfMass{};
	fp mass{};
	static constexpr fp theta = 1;
	BarnesHutTile* children[8]{};
	std::vector<BarnesHutTile*> occupiedChildren{};
	Square3 bounds;
	int bodyCount = 0;
	inline BarnesHutTile(cSquare3& bounds = Square3()) :bounds(bounds)
	{
	}
	constexpr int getChildIndex(const RigidBody* body) const {
		int childIndex = 0;
		for (int i = 0; i < 3; i++)
		{
			childIndex |= (body->centerOfMass[i] > bounds.center[i]) << i;
		}
		return childIndex;
	}
	//accept any type, as long as it has a centerOfMass and a mass. this makes it work for both BarnesHutTiles and RigidBodies
	inline void AddQuadrant(const RigidBody* body)
	{
		int index = getChildIndex(body);

		cfp& childNodeSize = bounds.radius * 0.5;
		Square3 childNodeBounds = cSquare3(bounds.center - childNodeSize, childNodeSize);

		for (int i = 0; i < 3; i++)
		{
			childNodeBounds.center[i] += bounds.radius * (index >> i & 1);
		}

		BarnesHutTile* child = children[index];
		if (child == nullptr)
		{
			if (storageIndex >= storage.size()) {
				//if storage would be a vector, this'd corrupt the 'this' pointer because it's modifying the vector in which it lives itself
				storage.push_back(BarnesHutTile());
			}
			child = &storage[storageIndex];
			children[index] = child;
			occupiedChildren.push_back(child);
			child->reset(childNodeBounds);
			storageIndex++;
		}
		child->AddBodyUnsafe(body);

	}
	inline bool inBounds(cvec3& pos) const {
		for (int i = 0; i < 3; i++) {
			if (std::abs(pos[i] - centerOfMass[i]) > bounds.radius)return false;
		}
		return true;
	}
	inline void AddBody(const RigidBody* body) {
		if (inBounds(body->centerOfMass))
			AddBodyUnsafe(body);
	}
	inline void AddBodyUnsafe(const RigidBody* body)
	{
		bodyCount++;
		if (bodyCount > 1)
		{
			if (this->bodies.size())
			{
				if (bounds.radius < smallestTileSize || this->bodies[0]->centerOfMass == body->centerOfMass)
				{
					this->bodies.push_back(body);
					//simply dont add the molecule. when we would add it, we would endlessly create child tiles trying to separate the bodies.
					return;
				}
				else
				{
					for (const RigidBody* const& existingBody : bodies) {
						//separate both molecules into different quadrants
						AddQuadrant(existingBody);
						this->bodies.clear();

					}
				}
			}
			AddQuadrant(body);
			//choose quadrant
		}
		else
		{
			this->bodies = { body };
		}
	}
	inline void CalculateMassDistribution()
	{
		if (this->bodies.size()) {
			for (const RigidBody* body : this->bodies) {
				this->mass += body->mass;
				this->centerOfMass = body->centerOfMass * body->mass;
			}
		}
		else
			for (BarnesHutTile* child : occupiedChildren) {
				child->CalculateMassDistribution();
				mass += child->mass;
				centerOfMass += child->mass * child->centerOfMass;
			}
		centerOfMass /= mass;
	}
	inline vec3 CalculateForce(cvec3& targetPos, cfp& gravitationalConstant) const
	{
		constexpr fp softenRadius = 1;
		if (
			//s / r < t
			//square everything
			//(s * s) / (r * r) < (t * t)
			math::squared(bounds.radius * 2) / (centerOfMass - targetPos).lengthSquared() < math::squared(theta)
			)
		{
			return calculateAcceleration(targetPos, centerOfMass, mass, gravitationalConstant, softenRadius);
		}
		else if (bodies.size()) {
			vec3 totalForce{};
			for (const RigidBody* body : bodies) {
				totalForce += calculateAcceleration(targetPos, body->centerOfMass, body->mass, gravitationalConstant, softenRadius);
			}
			return totalForce;
		}
		else
		{
			vec3 totalForce{};
			for (const BarnesHutTile* child : occupiedChildren) {
				totalForce += child->CalculateForce(targetPos, gravitationalConstant);
			}
			return totalForce;
		}
	}
	//static vec3 CalculateAcceleration(vec3 pos0, vec3 pos1, double m1, double gravityConstant)
	//{
	//	return calculateAcceleration(pos0, pos1, m1, gravityConstant, )
	//	return calculateGravityMass((pos1 - pos0).lengthSquared(), m1, gravityConstant, 1);
	//	//vec3 difference = pos1 - pos0;
	//	//double distanceSquared = difference.lengthSquared();
	//	//if (distanceSquared == 0)
	//	//{
	//	//	return vec3();
	//	//}
	//	////normal = difference / sqrt (distanceSquared)
	//	////normal = diff / d
	//	////return gravityConstant / distanceSquared * m1 * normal
	//	////replace for normal
	//	////g / d * d * m1 * norm
	//	////g / d * d * m1 * (diff / d)
	//	////diff * ((m1 * g) / (d * d * d))
	//	//return difference * ((m1 * gravityConstant) / (distanceSquared * std::sqrt(distanceSquared)));
	//}
	inline ~BarnesHutTile() {
		//for (BarnesHutTile* child : children) {
		//	delete child;
		//}
	}
	inline void reset() {
		reset(bounds);
		storage.resize(0x1000);
		storageIndex = 0;
	}
	inline void reset(cSquare3& newBounds) {
		//for (BarnesHutTile*& child : children) {
		//	delete child;
		//	child = nullptr;
		//}
		bodies.clear();
		bodyCount = 0;
		centerOfMass = vec3();
		mass = 0;
		bounds = newBounds;
		std::fill(std::begin(children), std::end(children), nullptr);
		occupiedChildren.clear();
	}
};
//initialize
std::deque<BarnesHutTile> storage{};