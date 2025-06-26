#include "control.h"

#pragma once
struct Label :public control
{
	Label(const std::wstring& text = std::wstring());
};