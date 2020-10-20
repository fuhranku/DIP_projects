#include "History.h"


History::History() {

}

History::~History() {

}

void History::_do(Action action) {
	actions.push(action);
}

Action History::_undo() {
	Action action = actions.top();
	actions.pop();
	return action;
}
