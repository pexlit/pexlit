#pragma once
#include "math/vector/vectn.h"
template <typename outputType = int, typename InputType, fsize_t axisCount>
constexpr vectn<outputType, axisCount> floorVector(const vectn<InputType, axisCount>& vec)
{
	vectn<outputType, axisCount> result = vectn<outputType, axisCount>();
	for (fsize_t i = 0; i < axisCount; i++)
	{
		result[i] = (outputType)math::floor<outputType, InputType>(vec[i]);
	}
	return result;
}

template <typename outputType = int, typename InputType, fsize_t axisCount>
constexpr vectn<outputType, axisCount> ceilVector(const vectn<InputType, axisCount>& vec)
{
	vectn<outputType, axisCount> result = vectn<outputType, axisCount>();
	for (fsize_t i = 0; i < axisCount; i++)
	{
		result[i] = (outputType)math::ceil<outputType, InputType>(vec[i]);
	}
	return result;
}

// https://www.omnicalculator.com/math/angle-between-two-vectors
// a and b have to be NORMALIZED
template <typename T, fsize_t N>
inline fp angleBetween(const vectn<T, N>& a, const vectn<T, N>& b)
{
	return std::acos(vectn<T, N>::dot(a, b));
}

// y is greater at the top
template <typename T>
constexpr bool woundClockwise(const vect2<T>& a, const vect2<T>& b, const vect2<T>& c)
{
	// counter-clockwise
	const vect2<T>& dab = b - a;
	const vect2<T>& dac = c - a;
	const T& winding = dab.x * dac.y - dab.y * dac.x;
	return winding < 0;
}

template<typename T, fsize_t N, typename indexType>
constexpr vectn<T, N> multiDimensionalIndex(indexType singleDimensionalIndex, const vectn<T, N>& size) {
	if constexpr (N == 1) {
		return vectn<T, N>((T)singleDimensionalIndex);
	}
	else {
		vectn<T, N> index = vectn<T, N>();
		for (fsize_t axisIndex = 0; axisIndex < N; axisIndex++) {
			index[axisIndex] = singleDimensionalIndex % size[axisIndex];
			singleDimensionalIndex /= size[axisIndex];
		}
		return index;
	}
}

template <typename indexT, typename sizeT, fsize_t n, typename returnIndexType = std::conditional_t < 1 < n && sizeof(indexT) < 4, fsize_t, indexT > >
constexpr returnIndexType singleDimensionalIndex(const vectn<indexT, n>& index, const vectn<sizeT, n>& size) requires(std::is_integral_v<indexT>)
{
	if constexpr (n == 1)
	{
		return index.axis[0];
	}
	else if constexpr (n == 2)
	{
		return index.axis[0] + index.axis[1] * size.axis[0];
	}
	else
	{
		returnIndexType sdIndex = index[n - 1];
		for (fsize_t i = n - 2;; i--)
		{
			sdIndex *= size.axis[i];
			sdIndex += index.axis[i];
			if (i == 0)
			{
				return sdIndex;
			}
		}
		// can'T come here
		// assumeInRelease(false);
	}
}

template <typename T, fsize_t n>
constexpr bool inBounds(const vectn<T, n>& index, const vectn<T, n>& size)
{
	for (auto it : std::views::zip(index, size))
	{
		if (it.template val<0>() < 0 || it.template val<0>() >= it.template val<1>())
		{
			return false;
		}
	}
	return true;
}

template <typename T, fsize_t n>
constexpr vectn<T, n> reflect(const vectn<T, n>& vector, const vectn<T, n>& normal)
{
	return vector - (2 * vectn<T, n>::dot(vector, normal)) * normal;
}

//https://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
constexpr fp distanceToLineSegment(cvec2& lineStart, cvec2& lineEnd, cvec2& p)
{
	// Return minimum distance between line segment vw and point p
	cfp& l2 = (lineStart - lineEnd).lengthSquared(); // i.e. |lineEnd-lineStart|^2 -  avoid a sqrt
	if (l2 == 0.0)
		return (p - lineStart).length(); // lineStart == lineEnd case
	// Consider the line extending the segment, parameterized as lineStart + T (lineEnd - lineStart).
	// We find projection of point p onto the line.
	// It falls where T = [(p-lineStart) . (lineEnd-lineStart)] / |lineEnd-lineStart|^2
	// We clamp T from [0,1] to handle points outside the segment vw.
	cfp& T = math::clamp(vec2::dot(p - lineStart, lineEnd - lineStart) / l2, (fp)0, (fp)1);
	const cvec2& projection = math::lerp(lineStart, lineEnd, T); // Projection falls on the segment
	return (p - projection).length();
}

//originalDirection: the direction to make perpendicular.
//planeNormal: the normal of the plane on which the originalDirection should lie
inline vec3 makePerpendicular(cvec3& originalDirection, cvec3& planeNormal) {
	cvec3 thirdAxis = vec3::cross(planeNormal, originalDirection).normalized();
	return vec3::cross(thirdAxis, planeNormal).normalized();
}

//originalDirection: the direction to make perpendicular.
//planeAxis#: the axes of the plane on which the originalDirection should lie
inline vec3 makePerpendicular(cvec3& originalDirection, cvec3& planeAxis1, cvec3& planeAxis2) {
	//we got two of those, so let's find the perfect perpendicular position.
	vec3 perp = vec3::cross(planeAxis1, planeAxis2).normalized();
	//when it's the other side, flip
	return perp * math::sign(vec3::dot(perp, originalDirection));
}
namespace std {
	template<typename T, fsize_t N>
	struct hash<vectn<T, N>> {
		size_t operator()(const vectn<T, N>& v) const noexcept {
			constexpr size_t prime = 92821ULL;
			size_t result = (size_t)v[0];
			for (int i = 1; i < N; i++) {
				result *= prime;
				result += v[i];
			}
			return result;
		}
	};
};