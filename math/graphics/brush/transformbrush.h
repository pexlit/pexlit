#pragma once
constexpr int brushFixMaxTextureSize = 0x1000;
constexpr int brushFixMaxTextureScaleDivider = 0x1000;//for extreme mipmapping
constexpr int brushFixMaxTextureMargin = 0x2;
constexpr fp brushFixTranslateAmount = 1.0 / brushFixMaxTextureSize / brushFixMaxTextureMargin;
constexpr fp brushFixScaleFactor = (1.0 - (brushFixTranslateAmount / brushFixMaxTextureSize));

constexpr mat3x3 brushFixTransform = mat3x3();//mat3x3::translate(cvec2(brushFixTranslateAmount));
//	mat3x3::cross(
//	mat3x3::translate(cvec2(1.0 / (fp)0x10000)), //translate the point a bit forward so when it rotates it won'T access the end of the array (it's like the pixels are in the center)
//	mat3x3::scale(cvec2(0xffffff / (fp)0x1000000)) //scales a bit so the last pixels don'T get out of bounds
//);
//transforms a point (after offsetting it a bit for rotation) and then calls a base brush
template<ValidBrush brush0Type>
struct transformBrush final : public ColorBrush
{
	//private:
		//transform: from screen pixel (or other transform) to texture pixel
	mat3x3 modifiedTransform;//cannot access the transform, as it is modified
	//public:
	const brush0Type& baseBrush;
	transformBrush(const mat3x3& transform, const brush0Type& baseBrush) :
		modifiedTransform(mat3x3::cross(brushFixTransform, transform)),//transformed pixel -> scale (make smaller), translate
		baseBrush(baseBrush)
	{
		vec2 add = vec2();
		for (int i = 0; i < 2; i++) {
			vec2 testSize = vec2(1);//directionVectors2D[i * 2 + 1];
			vec2 result = transform.multSizeMatrix(testSize);
			if (result[i] < 0) {
				//round up
				add[i] -= 1.0 / 0x1000;
			}
		}
		this->modifiedTransform = mat3x3::cross(mat3x3::translate(add), transform);
	}

	inline color getValue(cvec2& pos) const
	{
		//if constexpr (std::is_integral_v<brush0Type::InputType>) {
		//vec2 baseBrushPos = transform.multPointMatrix(pos);
		//for (int i = 0; i < 2; i++) {
		//	baseBrushPos[i] -= (roundUp[i] & (baseBrushPos[i] == (int)baseBrushPos[i]));
		//	//if (roundUp[i] && baseBrushPos[i] == (int)baseBrushPos[i]) {
		//	//	baseBrushPos[i]--;
		//	//}
		//}
		//for (int i = 0; i < 2; i++) {
		//	if (roundUp[i] && baseBrushPos[i] == (int)baseBrushPos[i]) {
		//		baseBrushPos[i]--;
		//	}
		//}
		//if (pos.y == (int)pos.y) {
		//	pos.y--;
		//}
	//}
		//return baseBrush.getValue(baseBrushPos);
		return baseBrush.getValue((typename brush0Type::InputType)modifiedTransform.multPointMatrix(pos));
	}
	struct Iterator : public RowIterator<transformBrush<brush0Type>> {
		typedef transformBrush<brush0Type> brushType;
		typedef RowIterator<brushType> base;
		const vec2 step;
		constexpr Iterator(const brushType& brush, cvec2& position) :
			base(brush, brush.modifiedTransform.multPointMatrix(position)),
			step(brush.modifiedTransform.getStep(axisID::x))
		{
		}
		//for when anything needs to be incremented without getting value
		constexpr Iterator& operator++() {
			base::position += step;
			return *this;
		}
		constexpr typename brush0Type::ResultingType operator*() const {
			return base::brush.baseBrush.getValue((typename brush0Type::InputType)base::position);
		}
	};
	constexpr Iterator getIterator(cvec2& pos) const {
		return Iterator(*this, pos);
	}
};

