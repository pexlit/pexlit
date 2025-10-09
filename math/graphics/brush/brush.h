#pragma once
#include "math/graphics/color/color.h"
//#include "interface/idestructable.h"
#include <type_traits>

template<typename BrushType>
concept ValidBrush = requires {
	typename BrushType::InputType;
	typename BrushType::ResultingType;
};

//old way:
//https://devblogs.microsoft.com/oldnewthing/20190710-00/?p=102678
//template<typename, typename = void>
//constexpr bool is_brush_v = false;
//
//template<typename T>
//constexpr bool is_brush_v
//<T, std::void_t<typename T::ResultingType>> = true;

#pragma once
template<ValidBrush BrushType>
struct RowIterator;
template <typename ResultingType, typename InputType>
struct Brush
{
	//add typedef for template argument, so the template parameters can be read from the outside.
	typedef InputType InputType;
	typedef ResultingType ResultingType;
	template <ValidBrush DerivedType>
	constexpr auto getIterator(this const DerivedType& self, const InputType& position);
	template <ValidBrush DerivedType>
	constexpr ResultingType getValue(this const DerivedType& self, const InputType& position);
};
template <ValidBrush BrushType>
struct RowIterator {
	const BrushType& brush;
	BrushType::InputType position;
	RowIterator(const BrushType& brush, const BrushType::InputType& position) :brush(brush), position(position) {}
	template <typename DerivedType>
	constexpr DerivedType& operator++(this DerivedType& self) {
		self.position.x++;
		return self;
	}
	template <typename DerivedType>
	constexpr BrushType::ResultingType operator*(this const DerivedType& self) {
		return self.brush.getValue(self.position);
	}
};

typedef Brush<color, vec2> ColorBrush;
typedef Brush<color, vect2<fsize_t>> colorBrushSizeT;

//template <typename, typename = void>
//constexpr bool is_brush_v = false;
//template <typename T, typename = T::ResultingType>
//constexpr bool is_brush_v = true;


//template<typename r, typename i>
//constexpr bool is_brush_v<brush<r, i>> = std::is_base_of<brush<r, i>>;


//struct colorBrushSizeT : public brush<color, vect2<fsize_t>>
//{
//	//typedef vect2<fsize_t> InputType;
//	//typedef color ResultingType;
//	//inline virtual color getValue(cvect2<fsize_t>& pos) const override = 0;
//
//	//inline virtual color getValue(cvec2& pos) const
//	//{
//	//
//	//	if constexpr (isDebugging)
//	//	{
//	//		if (pos.x < 0)
//	//		{
//	//			return colorPalette::red;
//	//		}
//	//		else if (pos.y < 0)
//	//		{
//	//			return colorPalette::green;
//	//		}
//	//	}
//	//	else 
//	//	{
//	//		assumeInRelease((pos.x >= 0) && (pos.y >= 0));
//	//	}
//	//	return getValue(floorVector<fsize_t>(pos));
//	//}
//};

template<typename ResultingType, typename InputType>
template<ValidBrush DerivedType>
constexpr auto Brush<ResultingType, InputType>::getIterator(this const DerivedType& self, const InputType& position)
{
	return RowIterator<DerivedType>(self, position);
}

template<typename ResultingType, typename InputType>
template<ValidBrush DerivedType>
constexpr ResultingType Brush<ResultingType, InputType>::getValue(this const DerivedType& self, const InputType& position) {
	auto iterator = self.getIterator(position);
	return *iterator;
}