#pragma once
#include "include/math/graphics/brush/brush.h"

template<ValidBrush brush0Type, ValidBrush brush1Type, ValidBrush DerivedCombineBrushType>
struct CombineBrushIterator;

template <typename brush0Type, typename brush1Type>
using CombinedInputType = std::conditional_t<
	//if both input types are the same, we accept the shared type
	std::is_same_v<typename brush0Type::InputType, typename brush1Type::InputType>, typename brush0Type::InputType,
	//else, it's vec2 (one of both must be vec2)
	vec2 > ;

template <ValidBrush brush0Type, ValidBrush brush1Type>
struct combinebrush : public Brush<color, CombinedInputType<brush0Type, brush1Type>>
{
	typedef	CombinedInputType<brush0Type, brush1Type> InputType;

	template<ValidBrush DerivedType>
	CombineBrushIterator<brush0Type, brush1Type, DerivedType> getIterator(this const DerivedType& self, const InputType& pos);
};
template<ValidBrush brush0Type, ValidBrush brush1Type, ValidBrush DerivedCombineBrushType>
struct CombineBrushIterator {
	const DerivedCombineBrushType& brush;
	decltype(brush.brush0.getIterator(typename brush0Type::InputType())) iterator0;
	decltype(brush.brush1.getIterator(typename brush1Type::InputType())) iterator1;
	CombineBrushIterator(const DerivedCombineBrushType& brush, const DerivedCombineBrushType::InputType& pos) :
		brush(brush),
		iterator0(brush.brush0.getIterator(typename brush0Type::InputType(pos))),
		iterator1(brush.brush1.getIterator(typename brush1Type::InputType(pos))) {

	}
	void operator++() {
		++iterator0;
		++iterator1;
	}
	color operator*() const {
		return brush.combine(*iterator0, *iterator1);
	}
};
template < ValidBrush brush0Type, ValidBrush brush1Type>
template<ValidBrush DerivedType>
CombineBrushIterator<brush0Type, brush1Type, DerivedType> combinebrush< brush0Type, brush1Type>::getIterator(this const DerivedType& self, const combinebrush< brush0Type, brush1Type>::InputType& pos) {
	return CombineBrushIterator<brush0Type, brush1Type, DerivedType>(self, pos);
}
//constexpr CombineBrushIterator<brush0Type, brush1Type, DerivedType> combinebrush< brush0Type, brush1Type>::getIterator(this const DerivedType& self, const InputType& pos);
//{
//	return Iterator<brush0Type, brush1Type, DerivedType>(self, pos);
//}
