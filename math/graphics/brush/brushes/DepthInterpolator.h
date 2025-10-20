#pragma once
#include "math/graphics/brush/brush.h"
#include "math/vector/vectorfunctions.h"

struct LinearDepthInterpolator : public Brush<fp, vec2> {
	fp inverseDepthAt00;
	vec2 inverseDepthPerStep;
	vec3 points[2];
	LinearDepthInterpolator(vec3 points[2]) {
		//pixels almost never get drawn on the line. the line is drawn between floored positions.
		vec2 flooredDifference = floorVector<fp>(points[1]) - floorVector<fp>(points[0]);
		//if (flooredDifference.x == 0 && flooredDifference.y == 0) {
		//	inverseDepthAt00 = math::maximum(points[0].z, points[1].z);
		//	inverseDepthPerStep = vec2();
		//}
		//else {
		fp inverseDepth0 = (fp)1.0 / points[0].z, inverseDepth1 = (fp)1.0 / points[1].z;

		//also, the line might be aligned with the Y-axis. therefore, lets use both x and y in the formula.
		//y = 1/(ax + b)
		inverseDepthPerStep = (inverseDepth1 - inverseDepth0) / flooredDifference;

		//now both axes account for the steps. lets give them some weight.
		//the axis with the greater value is of more importance.
		//normalize it so the sum is equal to 1 (taxicab)
		inverseDepthPerStep *= flooredDifference / (math::absolute(flooredDifference[0]) + math::absolute(flooredDifference[1]));
		for (int axis = 0; axis < 2; axis++) {
			if (!flooredDifference[axis]) {
				inverseDepthPerStep[axis] = 0;
			}
		}
		inverseDepthAt00 = inverseDepth0 - (vec2(points[0]) * inverseDepthPerStep).sum();
		//}

	}
	struct Iterator : public RowIterator<LinearDepthInterpolator> {
		typedef LinearDepthInterpolator brushType;
		typedef RowIterator<LinearDepthInterpolator> base;
		fp value;
		constexpr Iterator(const brushType& brush, cvec2& position) :
			base(brush, position), value(brush.inverseDepthAt00 + (brush.inverseDepthPerStep * position).sum())
		{

		}
		constexpr Iterator& operator++() {
			value += base::brush.inverseDepthPerStep.x;
			return *this;
		}

		constexpr typename brushType::ResultingType operator*() const {
			return (fp)1 / value;
		}
	};
	constexpr Iterator getIterator(cvec2& pos) const {
		return Iterator(*this, pos);
	}
};

