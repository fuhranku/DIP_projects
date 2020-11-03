#pragma once
#include "Image.h"
#include "DIPlib.h"

class Action
{
public:
	Action();
	Action(Image* image);
	~Action();

	Image* image;
};
