#pragma once
#include <math/vector/vectn.h>
#include <math/vector/vectorfunctions.h>
#include <math/quaternion/Quaternion.h>
struct RigidBody {
	vec3 centerOfMass;
	fp mass;
	vec3 velocity;
	Quaternion angularVelocity = Quaternion::identity();
	Quaternion rotation{};
	RigidBody() : centerOfMass{}, mass{} {}
	RigidBody(cvec3& centerOfMass, cfp& mass, cvec3& velocity = {}, Quaternion rotation = Quaternion::identity()) : centerOfMass(centerOfMass), mass(mass), velocity(velocity), rotation(rotation) {

	}
	void ApplyForce(vec3 force, vec3 location) {
		//https://stackoverflow.com/questions/36784456/calculating-angular-velocity-after-a-collision
		cvec3& toCenter = centerOfMass - location;
		cfp& distanceToCenter = toCenter.length();
		vec3 directionToCenter = toCenter / distanceToCenter;
		cfp& angleDifference = angleBetween(force.normalized(), directionToCenter);
		cfp& magnitude = force.length();
		//split force into velocity and angular force
		//velocity force directs to the center
		cfp& velocityPart = std::cos(angleDifference) * magnitude;

		if (angleDifference > 0) {
			cfp& angularPart = std::sin(angleDifference) * magnitude;
			fp angularAcceleration = angularPart / (distanceToCenter * mass);
			vec3 perpendicularVector = vec3::cross(force, directionToCenter).normalized();
			angularVelocity += Quaternion::axisAngle(perpendicularVector, angularAcceleration);
		}
		cvec3& acceleration = (velocityPart / mass) * directionToCenter;
		velocity += acceleration;
	}
};