#include "Action.h"


Action::Action(unsigned char* imageData, Image* image) {
	this->imageData = imageData;
	this->image = image;
}

Action::~Action() {

}
