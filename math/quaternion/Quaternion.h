#pragma once
#include <globalFunctions.h>
#include <math/vector/vectn.h>
#include <math/vector/vectorfunctions.h>
struct Quaternion :vec4 {
	using vec4::vectn;
	constexpr Quaternion(const vec4& vec) : vectn(vec.axis){}
	//angle in radians!
	static constexpr Quaternion axisAngle(cvec3& axis, cfp& angle) {
		cfp& half = angle * 0.5;
		return Quaternion(std::sin(half) * axis, std::cos(half));
	}
	static constexpr Quaternion identity() {
		return { 0,0,0,1 };
	}
	// Quaternion multiplication. last * first
	constexpr Quaternion operator*(const Quaternion& other) const {
		return {
			w * other.x + x * other.w + y * other.z - z * other.y,//x
			w * other.y - x * other.z + y * other.w + z * other.x,//y
			w * other.z + x * other.y - y * other.x + z * other.w,//z
			w * other.w - x * other.x - y * other.y - z * other.z //w
		};
	}
	//find the difference between two quaternions
	//from and to have to be normalized!
	static constexpr Quaternion between(const Quaternion& from, const Quaternion& to) {
		return from * to.conjugate();
	}
	// Conjugate (inverse if it's a unit quaternion)
	constexpr Quaternion conjugate() const {
		//for inverse, either flip sign of the w, or flip sign of the x, y and z
		//but for conjugate, flip xyz. so let's flip xyz
		return { -x, -y, -z, w };
	}
	constexpr Quaternion inverse() const {
		return Quaternion(conjugate() / lengthSquared());
	}
	// Rotate a vector by this quaternion: p' = q * (0,p) * inv(q)
	constexpr vec3 rotate(const vec3& point) const {
		Quaternion p_quat{ point.x, point.y, point.z,0 };
		//q * p * q-1
		Quaternion result = (*this) * p_quat * this->conjugate();
		return { result.x, result.y, result.z };
	}
	constexpr vec3 rotateInverse(cvec3& point) const {
		Quaternion p_quat{ point.x, point.y, point.z,0 };
		//inversion just depends on the order of operands
		//q-1 * p * q
		Quaternion result = this->conjugate() * p_quat * (*this);
		return { result.x, result.y, result.z };
	}

	static constexpr Quaternion moveDirection(cvec3& sourceDirection, cvec3& targetDirection) {
		//m->rotation = Quaternion::between(m->rotation, )
		//calculate rotation normal
		vec3 rotationNormal = vec3::cross(sourceDirection, targetDirection).normalized();
		//now rotate over normal. angle can always be positive, due to how the right handed system works
		return Quaternion::axisAngle(rotationNormal, angleBetween(sourceDirection, targetDirection));
	}
	using vec4::normalize;
	using vec4::normalized;
};