#pragma once
#include "brush/brushes/solidColorBrush.h"
#include "brush/brushes/colorMultiplier.h"
#include "brush/brushes/colorMixer.h"
#include "brush/brushes/squareInterpolator.h"
#include "brush/brushes/alphaMask.h"


// fill a row of pixels with a brush
template <typename T, ValidBrush brush0Type>
inline void fillRowUnsafe(const array2d<T>& array, cfsize_t& rowY, cfsize_t& minX, cfsize_t& maxX, const brush0Type& b)
{
	typedef typename brush0Type::InputType vectorType;
	typedef typename vectorType::axisType axisType;
	const vectorType& pos = vectorType((axisType)minX, (axisType)rowY);

	T* const rowPtr = array.baseArray + rowY * array.size.x;
	T* const endPtr = rowPtr + maxX;
	auto it = b.getIterator(pos);

	for (T* ptr = rowPtr + minX; ptr < endPtr; ptr++, ++it)
	{
		*ptr = *it;
	}
}
//template <typename T, ValidBrush brush0Type>
//inline void fillRowUnsafe(const array2d<T>& array, cfsize_t& rowY, cfsize_t& minX, cfsize_t& maxX, const transformBrush<brush0Type>& b)
//{
//	T* const rowPtr = array.baseArray + rowY * array.size.x;
//	T* const endPtr = rowPtr + maxX;
//
//	vec2 pos = b.modifiedTransform.multPointMatrix(vec2((fp)minX, (fp)rowY));
//	cvec2& step = b.modifiedTransform.getStep(axisID::x);
//
//	for (T* ptr = rowPtr + minX; ptr < endPtr; ptr++, pos += step)
//	{
//		*ptr = b.baseBrush.getValue((typename brush0Type::InputType)pos);
//	}
//}

template <typename T, ValidBrush brush0Type, ValidBrush brush1Type>
inline void fillRowUnsafe(const array2d<T>& array, cfsize_t& rowY, cfsize_t& minX, cfsize_t& maxX, const colorMixer<brush0Type, brush1Type>& b)
	requires(std::is_base_of_v<array2d<T>, brush1Type >)
{
	if (&b.brush1 == &array)
	{
		T* const rowPtr = array.baseArray + rowY * array.size.x;
		T* const endPtr = rowPtr + maxX;

		typedef typename brush0Type::InputType vectorType;
		typedef typename vectorType::axisType axisType;

		vectorType pos = vectorType((axisType)minX, (axisType)rowY);

		auto it = b.brush0.getIterator(pos);
		for (T* ptr = rowPtr + minX; ptr < endPtr; ptr++, ++it)
		{
			ccolor& topColor = *it;
			*ptr = topColor.a() == color::maxValue ? topColor : topColor.a() ? transitionColor(topColor, *ptr)
				: *ptr;
		}
	}
	else
	{
		fillRowUnsafe<T, colorMixer<brush0Type, brush1Type>>(array, rowY, minX, maxX, b);
	}
}

template <typename T, ValidBrush brush0Type>
inline void fillRow(const array2d<T>& array, cint& rowY, cint& minX, cint& maxX, const brush0Type& b)
{
	if ((rowY >= 0) && (rowY < (int)array.size.y) && (maxX > 0))
	{
		fillRowUnsafe(array, rowY, (fsize_t)math::maximum(minX, 0), math::minimum((fsize_t)maxX, array.size.x), b);
	}
}

template <typename T, ValidBrush brush0Type>
inline void fillRow(const array2d<T>& array, cint& rowY, cfp& minX, cfp& maxX, const brush0Type& b)
{
	// ceil rule
	fillRow(array, rowY, math::maximum((int)math::ceil(minX), 0), math::minimum((int)math::ceil(maxX), (int)array.size.x), b);
}

