#include "History.h"

std::stack<Action*> History::_undoStack;
std::stack<Action*> History::_redoStack;

History* History::_history = NULL;
/**
* Creates an instance of the class
*
* @return the instance of this class
*/
History* History::GetInstance() {
	if (!_history)   // Only allow one instance of class to be generated.
		_history = new History();
	return _history;
}

History::History() {}

History::~History() {
}

void History::_undo(Image* image) {
	// Revert states if applies (before undoing)

	// Push current Action to redo stack
	Image* savedImage = new Image();
	Image::cloneImage(image, savedImage);
	Action* current = new Action(savedImage);
	_redoStack.push(current);

	// Retrieve and pop undo Action
	Action* top = _undoStack.top();
	_undoStack.pop();
	
	// Update current image state
	Image::cloneImage(top->image, image);

	// Update GPU texture
	DIPlib::UpdateTextureData(image);

	// Revert states if applies (after undoing)
	if (image->currentFilter == IMG_TRANSFORM_ROTATE)
		image->canvas.Update(image->width, image->height);
}

void History::_redo(Image* image) {
	// Restore states if applies (before redoing)

	// Push current Action to undo stack
	Image* savedImage = new Image();
	Image::cloneImage(image, savedImage);
	Action* current = new Action(savedImage);
	_undoStack.push(current);

	// Retrieve and pop redo Action
	Action* top = _redoStack.top();
	_redoStack.pop();

	// Update current image state
	Image::cloneImage(top->image, image);

	// Update GPU texture
	DIPlib::UpdateTextureData(image);

	// Restore states if applies (after redoing)
	if (image->currentFilter == IMG_TRANSFORM_ROTATE)
		image->canvas.Update(image->width, image->height);

}


void History::PushAction(Image* image) {
	Image* savedImage = new Image();
	Image::cloneImage(image, savedImage);
	Action* current = new Action(savedImage);
	_undoStack.push(current);
	// Clear redo stack
	_redoStack = std::stack<Action*>();
}
