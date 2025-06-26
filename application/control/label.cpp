#include "label.h"
Label::Label(const std::wstring& text) :control()
{
	borderSize = 0;
	backGroundColor = colorPalette::transparent;
	this->text = text;
}