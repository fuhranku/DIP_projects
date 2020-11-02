#include "Action.h"

Action::Action() {}

Action::Action(Image* image) {
	this->image = new Image();
	Image::cloneImage(image, this->image);
}

Action::~Action() {

}

