#pragma once
#include "math/graphics/brush/brush.h"
#include "math/vector/vectorfunctions.h"
#include "math/graphics/brush/brushes/ApplyBrush.h"

template<ValidBrush DepthBrushType>
struct DepthVisualizer : public ApplyBrush<DepthBrushType> {
	using ApplyBrush<DepthBrushType>::ApplyBrush;

	color apply(cfp& depth) const {
		//0 = closest, 1 = furthest
		return color(0, 0, depth * 0xff);
	}
};

