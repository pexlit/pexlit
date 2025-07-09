#include <math/vector/vectn.h>
#pragma once
template <typename T, fsize_t N>
struct Squaretn {
	vectn<T, N> center;
	T radius;
	constexpr Squaretn(const vectn<T, N>& center = vectn<T, N>(), const T& radius = 0) : center(center), radius(radius) {}
};

addTemplateTypes(Square);