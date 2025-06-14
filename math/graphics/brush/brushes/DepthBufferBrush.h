#include "array/arraynd/arraynd.h"
#include <math/graphics/brush/brush.h>
//modifies the depth buffer!
template<typename brush0Type, typename depthBrush0Type, typename brush1Type, typename depthBrush1Type>
struct DepthBufferBrush : colorBrushSizeT {
	//top brush
	const brush0Type& brush0;
	const depthBrush0Type& depthBuffer0;
	//bottom brush
	const brush1Type& brush1;
	const depthBrush1Type& depthBuffer1;

	DepthBufferBrush(const brush0Type& brush0, const depthBrush0Type& depthBuffer0, const brush1Type& brush1, const depthBrush1Type& depthBuffer1) :
		brush0(brush0), depthBuffer0(depthBuffer0), brush1(brush1), depthBuffer1(depthBuffer1) {

	}

	inline color getValue(cvect2<fsize_t>& pos) const
	{
		cfp& depth = depthBuffer0.getValue(pos);
		if (depth < depthBuffer1.getValueUnsafe(pos)) {
			depthBuffer1.setValueUnsafe(pos, depth);
			return brush0.getValue(pos);
		}
		else {
			return brush1.getValue(pos);
		}
	}
};