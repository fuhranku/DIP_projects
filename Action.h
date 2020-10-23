#pragma once
#include "Image.h"


class Action
{

public:
	Action(unsigned char* imageData, Image* image);
	~Action();

	Image* image;
	unsigned char* imageData;
};

