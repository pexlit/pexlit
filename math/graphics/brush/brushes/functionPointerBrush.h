#pragma once
#include "math/graphics/brush/brush.h"
template <typename resultingType, typename inputType>
struct functionPointerBrush final : public Brush<resultingType, inputType >
{

	//be sure to convert function pointers to a std::function!
	functionPointerBrush(const std::function<resultingType(inputType)>& function) :function(function) {}
	const std::function<resultingType(inputType)> function;

	inline color getValue(const inputType& pos) const
	{
		return function(pos);
	}
};