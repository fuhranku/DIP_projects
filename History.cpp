#include "History.h"


History::History() {

}

History::~History() {

}

void History::_do(unsigned char *imageData, Image image) {
	Action* action = new Action(imageData, image);
	actions.push(action);
}

Action* History::_undo() {
	Action *action = actions.top();
	actions.pop();
	return action;
}
