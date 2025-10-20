#pragma once
//this brush takes input from another brush and does something with it.
#include "include/math/graphics/brush/brush.h"

template<ValidBrush Brush0Type, ValidBrush DerivedApplyBrushType>
struct ApplyBrushIterator;
template <ValidBrush Brush0Type>
struct ApplyBrush : public Brush<color, typename Brush0Type::InputType>
{
	const Brush0Type& brush0;
	constexpr ApplyBrush(const Brush0Type& brush0) : brush0(brush0) {}
	template<ValidBrush DerivedType>
	constexpr ApplyBrushIterator<Brush0Type, DerivedType> getIterator(this const DerivedType& self, const Brush0Type::InputType& pos);
};
template<ValidBrush Brush0Type, ValidBrush DerivedApplyBrushType>
struct ApplyBrushIterator {
	const DerivedApplyBrushType& brush;
	decltype(brush.brush0.getIterator(typename Brush0Type::InputType())) iterator0;
	constexpr ApplyBrushIterator(const DerivedApplyBrushType& brush, const DerivedApplyBrushType::InputType& pos) :
		brush(brush),
		iterator0(brush.brush0.getIterator(typename Brush0Type::InputType(pos))) {

	}
	constexpr void operator++() {
		++iterator0;
	}
	constexpr color operator*() const {
		return brush.apply(*iterator0);
	}
};
template <ValidBrush Brush0Type>
template<ValidBrush DerivedType>
constexpr ApplyBrushIterator<Brush0Type, DerivedType> ApplyBrush<Brush0Type>::getIterator(this const DerivedType& self, const Brush0Type::InputType& pos) {
	return ApplyBrushIterator<Brush0Type, DerivedType>(self, pos);
}
//constexpr CombineBrushIterator<brush0Type, brush1Type, DerivedType> combinebrush< brush0Type, brush1Type>::getIterator(this const DerivedType& self, const InputType& pos);
//{
//	return Iterator<brush0Type, brush1Type, DerivedType>(self, pos);
//}
