#pragma once
#include "math/graphics/color/color.h"
#include "math/graphics/brush/brushes/combineBrush.h"
#include "math/graphics/color/colorFunctions.h"
template<typename brush0Type, typename brush1Type>
struct ColorMixerIterator;
template <ValidBrush brush0Type, ValidBrush brush1Type>
struct colorMixer final : public combinebrush<brush0Type, brush1Type>
{
	typedef combinebrush<brush0Type, brush1Type> base;
	//top brush
	const brush0Type& brush0;
	//bottom brush
	const brush1Type& brush1;

	colorMixer(const brush0Type& topBrush, const brush1Type& bottomBrush) : brush0(topBrush), brush1(bottomBrush) {}

	// inline static constexpr color getColor(ccolor& topColor, ccolor& bottomColor)
	//{
	//	//static functions
	//	return (topColor.a() == color::maxValue) ?
	//		topColor :
	//		(topColor.a() ? color::transition(topColor, bottomColor) : bottomColor);
	// }
	constexpr color combine(const color& topColor, const color& bottomColor) const {
		if (topColor.a() == color::maxValue)
		{
			return topColor;
		}
		else if (topColor.a())
		{
			return transitionColor(topColor, bottomColor);
		}
		else
		{
			return bottomColor;
		}
	}

	inline color getValue(const typename base::InputType& pos) const
	{
		ccolor& topColor = brush0.getValue((typename brush0Type::InputType)pos);
		if (topColor.a() == color::maxValue)
		{
			return topColor;
		}
		else
		{
			ccolor& bottomColor = brush1.getValue((typename brush1Type::InputType)pos);
			if (topColor.a())
			{
				return transitionColor(topColor, bottomColor);
			}
			else
			{
				return bottomColor;
			}
		}

		// the bottom color will be optimized away if the topcolor does not have transparency
		// return getColor(topBrush.getValue(pos), bottomBrush.getValue(pos));
	}
	constexpr ColorMixerIterator<brush0Type, brush1Type> getIterator(const combinebrush<brush0Type, brush1Type>::InputType& pos) const;
};

template<typename brush0Type, typename brush1Type>
struct ColorMixerIterator :public CombineBrushIterator<brush0Type, brush1Type, colorMixer<brush0Type, brush1Type>> {
	typedef CombineBrushIterator< brush0Type, brush1Type, colorMixer<brush0Type, brush1Type>> base;
	using base::base;
	constexpr color operator*() const {
		color topColor = *base::iterator0;

		if (topColor.a() == color::maxValue)
		{
			return topColor;
		}
		else
		{
			ccolor& bottomColor = *base::iterator1;
			if (topColor.a())
			{
				return transitionColor(topColor, bottomColor);
			}
			else
			{
				return bottomColor;
			}
		}
	}
};

template<ValidBrush brush0Type, ValidBrush brush1Type>
inline constexpr ColorMixerIterator<brush0Type, brush1Type> colorMixer<brush0Type, brush1Type>::getIterator(const combinebrush<brush0Type, brush1Type>::InputType& pos) const
{
	return ColorMixerIterator<brush0Type, brush1Type>(*this, pos);
}
