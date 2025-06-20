#include "bodypart2d.h"

bodyPart2D::bodyPart2D(crectangle2& textureRect, bodyPart2D* const& parent, cvec2& translate, cvec2& size, cvec2& rotationCenter, cfp& angle, cint& textureAngle, cbool& hasTransparency)
{
	this->rotationCenter = rotationCenter;
	this->translate = translate;
	this->parent = parent;
	this->size = size;
	this->angle = angle;
	this->textureRect = textureRect;
	this->textureAngle = textureAngle;
	this->hasTransparency = hasTransparency;
	CalculateTransform();
	this->children = std::vector<bodyPart2D*>();
}

void bodyPart2D::CalculateTransform()
{
	changed = false;

	std::vector<mat3x3> transforms = std::vector<mat3x3>();

	transforms.push_back(mat3x3::translate(translate));
	if (angle != 0)
	{
		transforms.push_back(mat3x3::rotate(angle));
	}

	if (flipY)
	{
		transforms.push_back(mat3x3::mirror(axisID::y, -rotationCenter.y + size.y * 0.5));
	}
	if (flipX)
	{
		transforms.push_back(mat3x3::mirror(axisID::x, -rotationCenter.x + size.x * 0.5));
	}

	applied = mat3x3::combine(transforms);

	scalecenter = mat3x3::fromRectToRotatedRect(textureRect, textureAngle, crectangle2(-rotationCenter, size));
}

void bodyPart2D::CalculateAllTransforms()
{
	CalculateTransform();
	for (bodyPart2D* child : children)
	{
		child->CalculateAllTransforms();
	}
}
mat3x3 bodyPart2D::getCumulativeTransform()
{
	if (changed)
	{
		CalculateTransform();
	}
	mat3x3 cumulativeTransform = applied;
	bodyPart2D* part = parent;
	while (part)
	{
		cumulativeTransform = mat3x3::cross(part->applied, cumulativeTransform);
		part = part->parent;
	}
	//then to the body
	return cumulativeTransform;
}
mat3x3 bodyPart2D::getCumulativeParentTransform() const
{
	return parent ? parent->getCumulativeTransform() : mat3x3();
}

mat3x3 bodyPart2D::getCumulativeTextureTransform()
{
	return mat3x3::cross(getCumulativeTransform(), scalecenter);
}

vec2 bodyPart2D::getRotationCenterPosition() const
{
	return getCumulativeParentTransform().multPointMatrix(translate);
}

bodyPart2D::~bodyPart2D()
{
	for (bodyPart2D* part : children)
	{

		delete part;
	}
}