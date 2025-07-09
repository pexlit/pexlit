#pragma once
#include "math/graphics/brush/brush.h"
template<typename ResultingType>
struct solidBrush
{
	//make template argument publicly readable
	typedef ResultingType ResultingType;
	typedef veci2 InputType;
	ResultingType value = ResultingType();
	solidBrush(const ResultingType& value) :value(value) {}
	//the input is useless. hopefully, the compiler discards it.
	template<typename valueType>
	constexpr ResultingType getValue(valueType) const noexcept
	{
		return value;
	}
	template<typename valueType>
	constexpr void setPosition(valueType) const noexcept
	{
	}
	constexpr void operator++() const noexcept {
	}
	constexpr ResultingType operator*() const noexcept {
		return value;
	}
	template<typename DerivedType, typename positionType>
	constexpr const solidBrush& getIterator(this const DerivedType& self, const positionType& position) {
		return self;
	}
};