#pragma once
#include "math/graphics/brush/brush.h"
template <ValidBrush brush0Type, ValidBrush brush1Type>
struct colorMaximizer final : public ColorBrush
{
	typedef std::conditional_t<std::is_same_v<typename brush0Type::InputType, typename brush1Type::InputType>, typename brush0Type::InputType, ColorBrush::InputType> InputType;
	const brush0Type &brush0 = nullptr;
	const brush1Type &brush1 = nullptr;
	colorMaximizer(const brush0Type &brush0, const brush1Type &brush1) : brush0(brush0), brush1(brush1) {}
	virtual inline color getValue(const InputType &pos) const
	{
		return color::maximizeColors(brush0.getValue((typename brush0Type::InputType)pos), brush1.getValue((typename brush1Type::InputType)pos));
	}
};