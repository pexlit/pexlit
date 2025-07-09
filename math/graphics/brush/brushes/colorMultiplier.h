#pragma once
#include "math/graphics/brush/brush.h"
#include "math/graphics/brush/brushes/combineBrush.h"
#include "math/graphics/brush/brushes/solidColorBrush.h"
template<ValidBrush brush0Type, ValidBrush brush1Type>
struct ColorMultiplierIterator;
template<ValidBrush brush0Type, ValidBrush brush1Type>
struct colorMultiplier final : public combinebrush<brush0Type, brush1Type>
{
	typedef combinebrush<brush0Type, brush1Type> base;
	const brush0Type& brush0;
	const brush1Type& brush1;
	colorMultiplier(const brush0Type& baseBrush, const brush1Type& multColors) : brush0(baseBrush), brush1(multColors) {}
	constexpr color combine(const color& original, const color& multiplyWith) const {
		return color::multiplyColors<4>(original, multiplyWith);
	}
	constexpr color getValue(const typename base::InputType& pos) const
	{
		const color& original = brush0.getValue((typename brush0Type::InputType)pos);
		const color& multiplyWith = brush1.getValue((typename brush1Type::InputType)pos);

		return combine(original, multiplyWith);
	}

	ColorMultiplierIterator<brush0Type, brush1Type> getIterator(const base::InputType& position) const;

};
template<ValidBrush brush0Type, ValidBrush brush1Type>
struct ColorMultiplierIterator : public CombineBrushIterator<brush0Type, brush1Type, colorMultiplier<brush0Type, brush1Type>> {
	typedef CombineBrushIterator<brush0Type, brush1Type, colorMultiplier<brush0Type, brush1Type>> base;
	using base::base;
	constexpr typename color operator*() const {
		color color0 = *base::iterator0;

		if (color0.a())
		{
			return base::brush.combine(color0, *base::iterator1);
		}
		else
		{
			return color0;
		}
	}
};

template<ValidBrush brush0Type, ValidBrush brush1Type, typename targetType>
inline void multiplyRectangle(crectangle2& rect, const brush0Type& baseBrush, const brush1Type& multiplyColorsWith, const targetType& renderTarget)
{
	const auto& multiplier = colorMultiplier<brush0Type, brush1Type>(baseBrush, multiplyColorsWith);
	fillRectangle(renderTarget, rect, multiplier);
}
template<ValidBrush brush0Type, typename targetType>
inline void multiplyRectangle(crectangle2& rect, const brush0Type& multiplyColorsWith, const targetType& renderTarget)
{
	multiplyRectangle(rect, renderTarget, multiplyColorsWith, renderTarget);
}

template<typename targetType>
inline void multiplyRectangle(crectangle2& rect, const color& multiplyColorsWith, const targetType& renderTarget)
{
	const solidColorBrush b = solidColorBrush(multiplyColorsWith);
	multiplyRectangle(rect, b, renderTarget);
}

template<ValidBrush brush0Type, ValidBrush brush1Type>
inline ColorMultiplierIterator< brush0Type, brush1Type> colorMultiplier<brush0Type, brush1Type>::getIterator(const base::InputType& position) const
{
	return ColorMultiplierIterator<brush0Type, brush1Type>(*this, position);
}
