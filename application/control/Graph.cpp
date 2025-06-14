#include "Graph.h"
#include <math/mattnxn.h>
#include <math/graphics/graphicsFunctions.h>
#include <math/graphics/brush/font/baseFont.h>

Graph::Graph(graphFunctionType graphFunction, crectangle2& graphRect) : graphFunction(graphFunction), graphRect(graphRect), control()
{
}

mat3x3 Graph::getGraphTransform() const
{
	crectangle2& targetRect = rectangle2(rect).expanded(-borderSize);
	return mat3x3::fromRectToRect(targetRect, graphRect);
}

void Graph::render(cveci2& position, const texture& renderTarget)
{
	control::render(position, renderTarget);
	crectangle2& targetRect = rectangle2(rect).expanded(-borderSize);
	fp renderX = targetRect.x;
	cfp& renderX1 = renderX + targetRect.w;
	fp graphX = graphRect.x;
	//+1 to include the end of the graph
	cvec2& graphStep = graphRect.size / (targetRect.size + 1);
	cfp& graphMax = graphRect.y + graphRect.h;
	cfp& renderMin = targetRect.y;
	cfp& renderMax = targetRect.y + targetRect.h;

	cmat3x3& controlToGraph = getGraphTransform();
	cmat3x3& graphToControl = controlToGraph.inverse();
	constexpr fp rasterPixelDistance = 0x40;
	cvec2& roughInterval = controlToGraph.multSizeMatrix(vec2(rasterPixelDistance));


	cvec2& graphPos1 = graphRect.pos1();
	//TODO: round interval
	for (int axis = 0; axis < 2; axis++) {
		//find closest interval
		int powerOf10 = math::floor(std::log10(roughInterval.x));
		constexpr fp validLineIntervals[]{
			1, 2, 5, 10
		};
		cfp& baseInterval = math::powSizeTSimple(10.0, powerOf10);
		fp interval = baseInterval;
		for (fp lineInterval : validLineIntervals) {
			interval = baseInterval * lineInterval;
			if (interval >= roughInterval[axis])break;
		}

		cint& otherAxis = 1 - axis;
		//+ interval because else texts would overlap
		for (fp graphPos = math::floor(graphRect.pos0[axis] + interval, interval); graphPos < graphPos1[axis]; graphPos += interval) {
			vec2 bottomPos = graphRect.pos0;
			bottomPos[axis] = graphPos;
			cvec2& startPos = graphToControl.multPointMatrix(bottomPos);
			vec2 endPos = startPos;
			endPos[otherAxis] = targetRect.pos1()[otherAxis];

			fillLineUnsafeCropped(renderTarget, startPos, endPos, brushes::white, rect);
			std::wstringstream stream;
			stream << std::fixed << std::setprecision(2) << graphPos;
			currentFont->DrawString(stream.str(), rectangle2(startPos, vec2(rasterPixelDistance, currentFont->fontSize)), renderTarget);
		}
	}


	fp lastValue = 0;
	for (; renderX < renderX1; renderX++, graphX += graphStep.x) {
		fp currentValue = math::mapValue(graphFunction(graphX), graphRect.y, graphMax, renderMin, renderMax);
		if (!isnan(currentValue) && !isnan(lastValue)) {
			if (renderX > targetRect.x) {
				fillLineUnsafeCropped(renderTarget, vec2(renderX - 1, lastValue), vec2(renderX, currentValue), brushes::red, targetRect);
			}
		}
		lastValue = currentValue;
	}

	//render dots
	for (cvec2& dotPosition : dots) {
		fillRectangle(renderTarget, rectangle2(graphToControl.multPointMatrix(dotPosition), vec2()).expanded(2), brushes::green);
		//renderTarget.setValue(graphToControl.multPointMatrix(dotPosition), colorPalette::green);
	}
}

void Graph::scroll(cveci2& position, cint& scrollDelta)
{
	//positive scrolldelta: zoom in
	//zoom in or out
	cvec2& graphPosition = getGraphTransform().multPointMatrix(position);
	//center around graphposition
	graphRect.pos0 -= graphPosition;
	//zoom in or out
	graphRect = graphRect.multiplied(pow((fp)0.75, scrollDelta));
	//go back
	graphRect.pos0 += graphPosition;
}
