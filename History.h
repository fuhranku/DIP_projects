#pragma once
#include <stack>
#include "Action.h"

class History
{
private:
	std::stack<Action*> actions;

public:
	History();
	~History();

	void _do(unsigned char* imageData, Image* image);
	Action* _undo();

};

