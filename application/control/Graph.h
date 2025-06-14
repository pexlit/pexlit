#include "control.h"
#include <math/mattnxn.h>
#pragma once
struct Graph :public control
{
	typedef std::function<fp(fp)> graphFunctionType;
	graphFunctionType graphFunction;
	rectangle2 graphRect;
	std::vector<vec2> dots{};
	Graph(graphFunctionType graphFunction, crectangle2& graphRect);
	//returns a matrix which transforms positions from the control rect to the graph rect
	mat3x3 getGraphTransform() const;
	virtual void render(cveci2& position, const texture& renderTarget) override;
	virtual void scroll(cveci2& position, cint& scrollDelta) override;
};