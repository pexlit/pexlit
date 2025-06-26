#pragma once
#include <math/vector/vectn.h>
struct PixelOrientation {
	vec2 halfScreenSize;
	//multiply this by a pixel position to get the slope of a ray with z 1 (unnormalized) going from that pixel
	fp pixelToSlope{};
	fp slopeToPixel{};
	vec2 fovSlope{};
	inline PixelOrientation(vec2 screenSize, fp verticalFov) : halfScreenSize(vec2(screenSize) * 0.5) {
		fovSlope.y = tan(verticalFov * 0.5);
		fovSlope.x = fovSlope.y * (halfScreenSize.x / halfScreenSize.y);

		//multiply this by a pixel position to get the slope of a ray with z 1 (unnormalized) going from that pixel
		pixelToSlope = fovSlope.y / halfScreenSize.y;
		slopeToPixel = halfScreenSize.y / fovSlope.y;
	}
	//returns a ray direction assuming that the camera is looking to +y with +x as right and +z as up
	constexpr vec3 getRayDirection(vec2 pixelPos) const {
		vec2 xyRay = (pixelPos - halfScreenSize) * pixelToSlope;
		return { xyRay.x, 1, xyRay.y };
	}
};

struct RotatedPixelOrientation :PixelOrientation {
	vec3 cameraForward;
	vect2<vec3> Step;
	RotatedPixelOrientation(vec2 screenSize, fp verticalFov, cmat3x3& cameraTransform) :PixelOrientation(screenSize, verticalFov) {
		cameraForward = cameraTransform.getStep<3>(axisID::y);
		Step.x = cameraTransform.getStep<3>(axisID::x) * pixelToSlope;
		Step.y = cameraTransform.getStep<3>(axisID::z) * pixelToSlope;
	}
	constexpr vec3 getRayDirection(vec2 pixelPos) const {
		pixelPos -= halfScreenSize;
		return cameraForward + Step.x * pixelPos.x + Step.y * pixelPos.y;
	}
};