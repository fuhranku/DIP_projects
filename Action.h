#pragma once
#include "Image.h"


class Action
{

public:
	Action();
	~Action();

	Image image;
	unsigned char* imageData;
};

