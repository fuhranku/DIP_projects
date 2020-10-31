#pragma once
#include <stack>
#include "Action.h"
#include "Image.h"

class History
{
private:
	static History* _history;
	History();
	~History();
public:
	static std::stack<Action*> _undoStack;
	static std::stack<Action*> _redoStack;
	static History* GetInstance();
	static void PushAction(Image* image);
	static void _redo(Image* image);
	static void _undo(Image* image);
};
