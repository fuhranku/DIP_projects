#pragma once
#include <stack>
#include "Action.h"

class History
{
private:
	std::stack<Action*> _doStack;
	std::stack<Action*> _undoStack;

public:
	History();
	~History();

	void _do(Image* image, ACTION_TYPE type);
	Action* _undo();

};

